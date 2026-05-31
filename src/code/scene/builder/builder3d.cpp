#include "builder3d.h"

#include "../quadmesh.h"
#include "../gmaterial.h"
#include "../effect/ssaomaterial.h"
#include "../../loader/assimpmodel.h"
#include "../../hdr/hdrtexture.h"
#include "../../hdr/hdrcube.h"

std::vector<std::pair<glm::vec3, glm::vec3>> calculatePoses(const float radius, const float count) {
    std::vector<std::pair<glm::vec3, glm::vec3>> poses;
    auto origo = glm::vec3(0.0f);
    auto limit = 2.0f * glm::pi<float>();
    auto x = 0.0f;
    while (x < limit) {
        auto rotation = glm::vec3(0.0f, glm::degrees(x), 0.0f);
        auto forward = glm::vec3(sin(x), 0.0f, cos(x));
        poses.emplace_back(origo + radius * -forward, rotation);
        x += limit/count;
    }
    return poses;
}

glm::vec3 calculateForward(const float pitch, const float yaw) {
    float x = glm::radians(pitch);
    float y = glm::radians(yaw);

    glm::vec3 forward;
    forward.x = sin(y) * cos(x);
    forward.y = sin(x);
    forward.z = -cos(y) * cos(x);
    return forward;
}

void Builder3D::setupSSBO() { //NOLINT
    //TODO: Some type of data structure for SSBOs so they don't just kind of exist
    glGenBuffers(1, &ssbo1);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 9 * 32 + sizeof(float) * 32, nullptr, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo1);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &ssbo2);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * 9, nullptr, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo2);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Builder3D::precompute() { //NOLINT
    setupSSBO();
    //Initializing the required shaders
    auto precomputeBatch = std::make_unique<ShaderBatch>(std::vector<ShaderProperty>{
        {"computeConvolution", "convolution-sh-cs.glsl"},
        {"computeSum",         "sum-sh-cs.glsl"},
        {"equirectangular",    "envmapped-vs.glsl", "equirectangular-fs.glsl"},
        {"convolution",        "envmapped-vs.glsl", "convolution-fs.glsl"},
        {"prefilter",          "envmapped-vs.glsl", "prefilter-fs.glsl"},
        {"brdf",               "quad-vs.glsl",      "convolution-brdf-fs.glsl"}
    });

    auto cubeCamera = std::make_unique<Camera>(std::vector{
        (*precomputeBatch)["equirectangular"],
        (*precomputeBatch)["convolution"],
        (*precomputeBatch)["prefilter"]},
        glm::vec3(0.0f)
    );

    //Converting the equirectangular maps to cubemaps
    {
        framebufferCube->bindTarget(1, GL_RGBA16F, GL_CLAMP_TO_EDGE, TRILINEAR);
        auto material = std::make_unique<Material>((*precomputeBatch)["equirectangular"]);
        auto quad     = std::make_unique<QuadMesh>((*precomputeBatch)["equirectangular"], material.get());
        auto hdrCube  = std::make_unique<HDRCube>((*precomputeBatch)["equirectangular"], quad.get(), cubeCamera.get());
        material->linkUniform("hdrTexture", hdrTexture);
        for (int i = 0; i < 6; i++) {
            framebufferCube->bindBuffer(i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            precomputeBatch->executeOne("equirectangular");
        }
    }

    //Experimental - Generating the irradiance map using Spherical Harmonics
    {
        miscellaneous.linkPrograms((*precomputeBatch)["computeConvolution"]);
        miscellaneous.linkUniform("envTexture", (*framebufferCube)[0]);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo1);
        (*precomputeBatch)["computeConvolution"]->dispatch(glm::uvec3(8, 4, 1), GL_SHADER_STORAGE_BARRIER_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo2);
        (*precomputeBatch)["computeSum"]->dispatch(glm::uvec3(1, 1, 1), GL_SHADER_STORAGE_BARRIER_BIT);

        miscellaneous.flush();
    }

    (*framebufferCube)[0]->makeMipmap();

    //Convoluting the previously converted cubemap
    {
        convolutedFBOCube->bindTarget(1, GL_RGBA16F, GL_CLAMP_TO_EDGE, BILINEAR);
        auto material = std::make_unique<Material>((*precomputeBatch)["convolution"]);
        auto quad     = std::make_unique<QuadMesh>((*precomputeBatch)["convolution"], material.get());
        auto hdrCube  = std::make_unique<HDRCube>((*precomputeBatch)["convolution"], quad.get(), cubeCamera.get());
        material->linkUniform("envTexture", (*framebufferCube)[0]);
        for (int i = 0; i < 6; i++) {
            convolutedFBOCube->bindBuffer(i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            precomputeBatch->executeOne("convolution");
        }
    }

    //Pre-filtering the previously converted cubemap
    {
        float roughness;
        prefilterFBOCube->bindTarget(1, GL_RGBA16F, GL_CLAMP_TO_EDGE, TRILINEAR);
        auto material = std::make_unique<Material>((*precomputeBatch)["prefilter"]);
        auto quad     = std::make_unique<QuadMesh>((*precomputeBatch)["prefilter"], material.get());
        auto hdrCube  = std::make_unique<HDRCube>((*precomputeBatch)["prefilter"], quad.get(), cubeCamera.get());
        material->linkUniform("envTexture", (*framebufferCube)[0]);
        material->linkUniform("roughness", &roughness);
        const unsigned int nMipLevel = 5;
        for (int mip = 0; mip < nMipLevel; mip++) {
            roughness = (float)mip / (float)(nMipLevel - 1);
            for (int i = 0; i < 6; i++) {
                prefilterFBOCube->bindBuffer((mip * 6) + i);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                precomputeBatch->executeOne("prefilter");
            }
        }
    }

    //Convoluting the BRDF and creating the LUT
    {
        integratedFBO->bindTarget(1, GL_RG16F, GL_CLAMP_TO_EDGE, BILINEAR);
        auto material = std::make_unique<Material>((*precomputeBatch)["brdf"]);
        auto quad     = std::make_unique<QuadMesh>((*precomputeBatch)["brdf"], material.get());
        auto model    = std::make_unique<Model>((*precomputeBatch)["brdf"], std::vector<Mesh*>{quad.get()});
        integratedFBO->bindBuffer(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        precomputeBatch->executeOne("brdf");
    }

    //Binding back the default framebuffer
    defaultFramebuffer->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Builder3D::buildPrograms() {
    gBufferBatch = new ShaderBatch({
        {"gbuffer",  "skinning-vs.glsl",  "gbuffer-fs.glsl"}  //G-Buffer program
    });

    defaultBatch = new ShaderBatch({
        {"maxblinn",  "quad-proc-vs.glsl", "maxblinn-fs.glsl"},       //Max-Blinn program
        {"pbr",       "quad-proc-vs.glsl", "cook-torrance-fs.glsl"}, //PBR program
    });

    backgroundBatch = new ShaderBatch({
        {"background", "envmapped-vs.glsl", "envmapped-fs.glsl"} //Background program
    });

    preProcBatch = new ShaderBatch({
        {"ssaoNoise", "quad-proc-vs.glsl", "ssao-noise-fs.glsl"}, //SSAO Noise program
        {"ssaoBlur",  "quad-vs.glsl", "ssao-blur-fs.glsl"}  //SSAO Blur program
    });

    postProcBatch = new ShaderBatch({
        {"copy",         "quad-vs.glsl", "extraction-fs.glsl"},    //Idle/Copy program
        {"downsampling", "quad-vs.glsl", "downsampling-fs.glsl"}, //Downsampling program
        {"upsampling",   "quad-vs.glsl", "upsampling-fs.glsl"},  //Upsampling program
        {"postproc",     "quad-vs.glsl", "postproc-fs.glsl"},   //Post-proc program
    });

    hotreload = (*defaultBatch)["pbr"];
    ssaoShader = (*preProcBatch)["ssaoNoise"];
    bloomShader = (*postProcBatch)["copy"];
}

void Builder3D::buildMeshes() {
    backgroundMesh = new QuadMesh((*backgroundBatch)["background"], backgroundMaterial);
    postProcMesh = new QuadMesh((*postProcBatch)["postproc"], postProcMaterial);
    gBufferMesh  = new QuadMesh(hotreload, gBufferMaterial);
    ssaoMesh = new QuadMesh(ssaoShader, ssaoMaterial);
    bloomMesh = new QuadMesh(bloomShader, bloomMaterial);
}

void Builder3D::buildMaterials() {
    hdrTexture = new HDRTexture("background/hdr/outdoor/mystic/blue_grotto_4k.hdr");
    backgroundMaterial = new Material((*backgroundBatch)["background"]);
    postProcMaterial = new Material((*postProcBatch)["postproc"]);
    gBufferMaterial  = new GMaterial(hotreload, gBufferFBO);
    ssaoMaterial = new SSAOMaterial(ssaoShader, gBufferFBO, 4, 24);
    bloomMaterial = new Material(bloomShader);
}

void Builder3D::buildModels() {
    models.push_back(new Model(hotreload, {gBufferMesh}));
    models.push_back(new Model(ssaoShader, {ssaoMesh}));
    models.push_back(new Model(bloomShader, {bloomMesh}));
    models.push_back(new Model((*postProcBatch)["postproc"], {postProcMesh}));
    models.push_back(new Model((*backgroundBatch)["background"], {backgroundMesh}));

    models.push_back(new AssimpModel((*gBufferBatch)["gbuffer"],
        "assimp/showroom/forgotten_knight/scene.gltf",
        glm::vec3(-12.0f, 0.2f, -16.0f),
          glm::vec3(2.85f, 2.85f, 2.85f),
        glm::vec3(0.0f, -100.0f, 0.0f),
        ALBEDO, NORMAL, METALLIC_ROUGHNESS, EMISSIVE));

    models.push_back(new AssimpModel((*gBufferBatch)["gbuffer"],
        "assimp/showroom/shadowflame_samurai/scene.gltf",
        glm::vec3(-14.5f, 0.3f, 5.0f),
          glm::vec3(4.0f, 4.0f, 4.0f),
          glm::vec3(0.0f, -190.0f, 0.0f),
        ALBEDO, NORMAL, METALLIC_ROUGHNESS, EMISSIVE));

    models.push_back(new AssimpModel((*gBufferBatch)["gbuffer"],
        "assimp/showroom/stelae_knight/scene.gltf",
        glm::vec3(-2.0f, 11.505f, -8.0f),
          glm::vec3(1.7f, 1.7f, 1.7f),
          glm::vec3(0.0f, -10.0f, 0.0f),
        ALBEDO, NORMAL, METALLIC_ROUGHNESS, EMISSIVE));

    models.push_back(new AssimpModel((*gBufferBatch)["gbuffer"],
    "assimp/showroom/medieval_arcade/scene.gltf",
    glm::vec3(0.0f),
      glm::vec3(5.0f),
    glm::vec3(0.0f, -100.0f, 0.0f),
        ALBEDO, NORMAL, METALLIC_ROUGHNESS, EMISSIVE));
}

void Builder3D::buildFramebuffers() {
    defaultFramebuffer  = new DefaultFramebuffer();
    framebufferCube     = new FramebufferCube(1024, 1024, (int)LoD + 1);
    convolutedFBOCube   = new FramebufferCube(64, 64, 1);
    prefilterFBOCube    = new FramebufferCube(128, 128, (int)LoD + 1);
    integratedFBO       = new Framebuffer(512, 512, 1);

    postProcFBO = new Framebuffer(WindowSize::width, WindowSize::height, 1);
    /*Scene*/
    postProcFBO->bindTarget(1, GL_R11F_G11F_B10F, GL_CLAMP_TO_EDGE, POINT);

    gBufferFBO = new Framebuffer(WindowSize::width, WindowSize::height, 1);
    /*Depth and Surface Normal*/
    gBufferFBO->bindTarget(1, GL_RGBA16F, GL_CLAMP_TO_EDGE, POINT);
    /*Albedo*/
    gBufferFBO->bindTarget(1, GL_RGBA8, GL_CLAMP_TO_EDGE, POINT);
    /*PBR Metallic and Roughness*/
    gBufferFBO->bindTarget(1, GL_RG8, GL_CLAMP_TO_EDGE, POINT);
    /*Emission*/
    gBufferFBO->bindTarget(1, GL_R11F_G11F_B10F, GL_CLAMP_TO_EDGE, POINT);

    ssaoFBO = new Framebuffer(WindowSize::width, WindowSize::height, 1);
    /*SSAO Noise and Blur*/
    ssaoFBO->bindTarget(2, GL_R8, GL_REPEAT, POINT);

    bloomFBO = new Framebuffer(WindowSize::width, WindowSize::height, bloomMip);
    bloomFBO->bindTarget(1, GL_R11F_G11F_B10F, GL_CLAMP_TO_EDGE, TRILINEAR);
}

void Builder3D::buildCamera() {
    camera = new CinematicCamera(
        {(*gBufferBatch)["gbuffer"],
                    (*defaultBatch)["maxblinn"],
                    (*defaultBatch)["pbr"],
                    (*preProcBatch)["ssaoNoise"],
                    (*backgroundBatch)["background"],
                    (*postProcBatch)["postproc"]},
        glm::vec3(3.25f, 12.5f, 13.8f),
        8.0f, -120.0f,
        8.0f, 2.75f
    );
    camera->Speed() = 4.0f;
    camera->linkUniform("invView",       &camera->InvView());
    camera->linkUniform("invProjection", &camera->InvProjection());
    camera->linkUniform("farPlane",      &camera->FarPlane());

    camera->addSequence(glm::vec3(-25.0f, 5.0f,   4.5f),  calculateForward(0.0f, 10.0f),   glm::vec2(  0.0f,  10.0f));
    camera->addSequence(glm::vec3(-26.0f, 18.0f,-17.5f),  calculateForward(-3.0f, 100.0f),  glm::vec2( -3.0f,  10.0f));
    camera->addSequence(glm::vec3(  9.5f, 20.0f, 19.0f),  calculateForward(0.0f, 10.0f),  glm::vec2(-30.0f, 190.0f));
    camera->addSequence(glm::vec3( 14.0f, 22.5f, -14.5f),  calculateForward(-31.5f, 235.0f),glm::vec2( -31.5f,145.0f));
    camera->addSequence(glm::vec3(-25.0f, 4.0f, 11.0f),  calculateForward(0.0f, 100.0f),glm::vec2( 25.0f,-80.0f));
    camera->addSequence(glm::vec3(-9.5f, 7.0f,  7.45f), calculateForward(0.0f, 10.0f),  glm::vec2(  -5.0f,190.0f));
}

void Builder3D::buildLights() {
    for (int i = 0; i < 2; i++) {
        lights.push_back(new FlickeringLight(
            {(*defaultBatch)["pbr"], (*defaultBatch)["maxblinn"]},
             glm::vec3(65.08125f, 18.09825f, 1.206f),
            glm::vec3(111.25f, 30.9375f, 2.0625f),
            glm::vec2(0.25f, 0.55f),
            "lights[" + std::to_string(i) + "]"));
    }

    for (int i = 2; i < 6; i++) {
        lights.push_back(new FlickeringLight(
            {(*defaultBatch)["pbr"], (*defaultBatch)["maxblinn"]},
             glm::vec3(4.9875f, 2.73f, 0.02625f),
            glm::vec3(14.25f, 7.8f, 0.075f),
            glm::vec2(0.05f, 0.25f),
            "lights[" + std::to_string(i) + "]"));
    }

    lights[0]->Position() = glm::vec4(-16.13f, 7.45f, -6.25f, 1.0f);
    lights[1]->Position() = glm::vec4(-6.04f, 7.45f, -21.65f, 1.0f);
    lights[2]->Position() = glm::vec4(-1.52f, 25.05f, -12.45f, 1.0f);
    lights[3]->Position() = glm::vec4(-2.3f, 25.05f, -8.05f, 1.0f);
    lights[4]->Position() = glm::vec4(-0.5f, 23.95f, -9.98f, 1.0f);
    lights[5]->Position() = glm::vec4(-3.38f, 24.15f, -10.5f, 1.0f);
}

void Builder3D::buildUniforms() {
    //Precomputing the required textures for IBL
    precompute();
    //Binding the appropriate textures for IBL
    miscellaneous.linkPrograms((*defaultBatch)["pbr"]);
    miscellaneous.linkUniform("PBRold", &pbrOld);
    miscellaneous.linkUniform("spherical", &spherical);
    miscellaneous.linkUniform("ambient", &ambient);
    miscellaneous.linkUniform("exposure", &exposure);
    miscellaneous.linkUniform("LoD", &LoD);
    miscellaneous.linkUniform("LuT", (*integratedFBO)[0]);
    miscellaneous.linkUniform("irradianceMap", (*convolutedFBOCube)[0]);
    miscellaneous.linkUniform("prefilterMap", (*prefilterFBOCube)[0]);
    miscellaneous.linkUniform("SSAO", [this] { return (*ssaoFBO)[1]; });

    miscellaneous.linkPrograms((*defaultBatch)["maxblinn"]);
    miscellaneous.linkUniform("SSAO", [this] { return (*ssaoFBO)[1]; });
    miscellaneous.linkUniform("ambient", &ambient);

    miscellaneous.linkPrograms((*preProcBatch)["ssaoNoise"]);
    miscellaneous.linkUniform("windowWidth",  &WindowSize::width);
    miscellaneous.linkUniform("windowHeight", &WindowSize::height);

    miscellaneous.linkPrograms((*preProcBatch)["ssaoBlur"]);
    miscellaneous.linkUniform("ssaoNoise", [this] { return (*ssaoFBO)[0]; });

    miscellaneous.linkPrograms((*postProcBatch)["copy"]);
    miscellaneous.linkUniform("scene", [this] { return (*postProcFBO)[0]; });

    miscellaneous.linkPrograms((*postProcBatch)["downsampling"], (*postProcBatch)["upsampling"]);
    miscellaneous.linkUniform("srcTexture", [this] { return (*bloomFBO)[0]; });
    miscellaneous.linkUniform("mipLevel", &bloomMipLevel);

    backgroundMaterial->linkUniform("envTexture", (*framebufferCube)[0]);

    postProcMaterial->linkUniform("bloom", &bloom);
    postProcMaterial->linkUniform("exposure", &exposure);
    postProcMaterial->linkUniform("rawTexture",  [this] { return (*postProcFBO)[0]; });
    postProcMaterial->linkUniform("bloomTexture",[this] { return (*bloomFBO)[0]; });
}

void Builder3D::handleInput(const std::set<unsigned int>& keysPressed) {
    if (keysPressed.contains('m') && timeSinceLastHotreload >= 1.0f) {
        auto newReload = hotreload == (*defaultBatch)["maxblinn"] ? (*defaultBatch)["pbr"] : (*defaultBatch)["maxblinn"];
        hotreload->unsubscribe(models[0]);
        models[0]->relink({newReload});
        hotreload = newReload;
        timeSinceLastHotreload = 0.0f;
    }

    if (keysPressed.contains('c') && timeSinceLastCinematic >= 1.0f) {
        camera->changeState();
        timeSinceLastCinematic = 0.0f;
    }

    if (keysPressed.contains('i') && timeSinceLastIrradianceChange >= 1.0f) {
        spherical = !spherical;
        timeSinceLastIrradianceChange = 0.0f;
    }

    if (keysPressed.contains('o') && timeSinceLastSSAOSwitch >= 1.0f) {
        ambient = !ambient;
        timeSinceLastSSAOSwitch = 0.0f;
    }

    if (keysPressed.contains('p') && timeSinceLastPBRSwitch >= 1.0f) {
        pbrOld = !pbrOld;
        timeSinceLastPBRSwitch = 0.0f;
    }

    if (keysPressed.contains('b') && timeSinceLastBloom >= 1.0f) {
        bloom = !bloom;
        timeSinceLastBloom = 0.0f;
    }
}

void Builder3D::draw(float dt, const std::set<unsigned int>& keysPressed) {
    defaultFramebuffer->resize(WindowSize::width, WindowSize::height);
    postProcFBO->resize(WindowSize::width, WindowSize::height);
    gBufferFBO->resize(WindowSize::width, WindowSize::height);
    ssaoFBO->resize(WindowSize::width, WindowSize::height);
    bloomFBO->resize(WindowSize::width, WindowSize::height);

    ThreadPool::processCompletedJobs();

    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glClearDepth(1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    handleInput(keysPressed);

    camera->move(dt, keysPressed);
    for (auto light : lights) {
        light->update(dt);
    }

    /* ===================================================== *
     * GEOMETRY PASS
     * ===================================================== */
    gBufferFBO->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    gBufferBatch->executeAll();

    /* ===================================================== *
     * PRE-PROCESS PASS
     * ===================================================== */
    glDisable(GL_DEPTH_TEST);
    ssaoFBO->bindBuffer(0);

    /* ---------------- SSAO NOISE ---------------- */
    ssaoFBO->lockTarget(0);
    glClear(GL_COLOR_BUFFER_BIT);
    ssaoShader->unsubscribe(models[1]);
    ssaoShader = (*preProcBatch)["ssaoNoise"];
    models[1]->relink({ssaoShader});
    preProcBatch->executeOne("ssaoNoise");

    /* ---------------- SSAO BLUR ---------------- */
    ssaoFBO->lockTarget(1);
    glClear(GL_COLOR_BUFFER_BIT);
    ssaoShader->unsubscribe(models[1]);
    ssaoShader = (*preProcBatch)["ssaoBlur"];
    models[1]->relink({ssaoShader});
    preProcBatch->executeOne("ssaoBlur");

    /* ===================================================== *
     * LIGHTING PASS
     * ===================================================== */
    postProcFBO->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT);
    gBufferFBO->syncDepth(postProcFBO);
    postProcFBO->bindBuffer(0);
    defaultBatch->executeAll();

    /* ---------------- BACKGROUND ---------------- */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    backgroundBatch->executeAll();

    /* ===================================================== *
     * BLOOM PASS
     * ===================================================== */
    bloomFBO->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    bloomShader->unsubscribe(models[2]);
    bloomShader = (*postProcBatch)["copy"];
    models[2]->relink({bloomShader});
    postProcBatch->executeOne("copy");

    /* ---------------- DOWNSAMPLING ---------------- */
    bloomShader->unsubscribe(models[2]);
    bloomShader = (*postProcBatch)["downsampling"];
    models[2]->relink({bloomShader});

    for (int mip = 1; mip < bloomMip; mip++) {
        bloomMipLevel = mip - 1;
        bloomFBO->bindBuffer(mip);
        glClear(GL_COLOR_BUFFER_BIT);
        postProcBatch->executeOne("downsampling");
    }

    /* ---------------- UPSAMPLING ---------------- */
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    bloomShader->unsubscribe(models[2]);
    bloomShader = (*postProcBatch)["upsampling"];
    models[2]->relink({bloomShader});

    {
        for (int mip = bloomMip - 2; mip >= 0; mip--) {
            bloomMipLevel = mip + 1;
            bloomFBO->bindBuffer(mip);
            postProcBatch->executeOne("upsampling");
        }
    }

    /* ===================================================== *
     * POST-PROCESS PASS
     * ===================================================== */
    defaultFramebuffer->bindBuffer(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_BLEND);
    postProcBatch->executeOne("postproc");

    /*Increase timers*/
    timeSinceLastHotreload += dt;
    timeSinceLastCinematic += dt;
    timeSinceLastIrradianceChange += dt;
    timeSinceLastSSAOSwitch += dt;
    timeSinceLastPBRSwitch += dt;
    timeSinceLastBloom += dt;
}