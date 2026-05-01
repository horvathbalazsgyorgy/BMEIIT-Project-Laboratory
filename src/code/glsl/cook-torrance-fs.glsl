#version 430 core

precision highp float;

in vec4 worldPosition;
in vec4 worldNormal;
in vec4 worldTangent;
in vec4 worldBitangent;
in vec3 viewDir;
in vec4 color;
in vec2 tex[4];

uniform struct{
    sampler2D albedoMap1;
    sampler2D normalMap1;
    sampler2D mrMap1;
    sampler2D aoMap1;

    int albedoMap1_uv;
    int normalMap1_uv;
    int mrMap1_uv;
    int aoMap1_uv;
} material;

uniform struct{
    vec4 position;
    vec3 emittance;
} lights[3];

uniform float exposure;
uniform float LoD;
uniform sampler2D LuT;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;

out vec4 fragmentColor;
const float PI = 3.14159265359;

//Normal Distribution function - Using the Trowbridge-Reitz GGX
float Distribution(float roughness, float cost){
    float a     = pow(roughness, 2.0);
    float a2    = pow(a, 2.0);
    float cos0  = pow(cost, 2.0);
    float denom = PI * pow(cos0 * (a2 - 1.0f) + 1.0f, 2.0);

    return a2 / denom;
}

//Geometry function - Using the Schlick-GGX approximation
float Geometry(float k, float cosa, float cosb){
    float vGGX = cosb / (cosb * (1.0f - k) + k);
    float lGGX = cosa / (cosa * (1.0f - k) + k);

    return vGGX * lGGX;
}

//Fresnel function - Using the Fresnek-Schlick approximation
vec3 Fresnel(vec3 F0, float cost){
    return F0 + (1.0f - F0) * pow(clamp(1.0f - cost, 0.0f, 1.0f), 5.0);
}

//Fresnel function - Using the Fresnek-Schlick approximation
vec3 FresnelRoughness(vec3 F0, float cost, float roughness){
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(clamp(1.0f - cost, 0.0f, 1.0f), 5.0);
}

vec3 CookTorranceBRDF(float ND, float G, vec3 F, float cosa, float cosb){
    vec3   DGF   = ND * G * F;
    float  denom = 4.0f * cosa * cosb + 0.0001f;
    return DGF / denom;
}

vec3 shadePunctual(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 radiance, vec3 albedo, float roughness, float metallic){
    //Angles
    vec3 halfway = normalize(lightDir + viewDir);
    float cosa = max(dot(normal, lightDir), 0.0f);
    float cosb = max(dot(normal, viewDir),  0.0f);
    float cost = max(dot(normal, halfway),  0.0f);

    //Fresnel component
    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);
    vec3 F = Fresnel(F0, max(dot(halfway, viewDir), 0.0));

    //Geometry component
    float k = pow(roughness + 1.0f, 2.0) / 8.0f;
    float G = Geometry(k, cosa, cosb);

    //Normal Distribution component
    float ND = Distribution(roughness, cost);

    //Cook-Torrance BRDF
    vec3 specularColor = CookTorranceBRDF(ND, G, F, cosa, cosb);

    //Reflectance/refraction values
    vec3 ks = F;
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic;

    return (kd * albedo / PI + specularColor) * radiance * cosa;
}

vec3 shadeIBL(vec3 normal, vec3 viewDir, vec3 albedo, float roughness, float metallic, float ao){
    float cosb = max(dot(normal, viewDir),  0.0f);
    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

    vec3 F  = FresnelRoughness(F0, cosb, roughness);
    vec3 ks = F;
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic;

    vec3 irradiance = texture(irradianceMap, normal).rgb;
    vec3 diffuse    = irradiance * albedo;

    vec3 reflection     = reflect(-viewDir, normal);
    vec3 prefilterColor = textureLod(prefilterMap, reflection, roughness * LoD).rgb;
    vec2 envBRDF  = texture(LuT, vec2(cosb, roughness)).rg;
    vec3 specular = prefilterColor * (F * envBRDF.x + envBRDF.y);

    return (kd * diffuse + specular) * ao;
}

void main(void) {
    //Stuff for normal mapping
    vec3 N = gl_FrontFacing ? normalize(worldNormal.xyz) : -normalize(worldNormal.xyz);
    vec3 T = normalize(worldTangent.xyz);
    vec3 B = normalize(worldBitangent.xyz);
    mat3 TBN = mat3(T, B, N);

    //Normals
    vec3 tangentNormal = texture(material.normalMap1, tex[material.normalMap1_uv]).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);
    vec3 normal   = normalize(TBN * tangentNormal);

    //Extracting material properties
    vec4 materialColor = texture(material.albedoMap1, tex[material.albedoMap1_uv]);
    if(materialColor.a < 0.1){
        discard;
    }
    vec3  albedo = materialColor.rgb * color.rgb;
    vec3  metallicRougness = texture(material.mrMap1, tex[material.mrMap1_uv]).rgb;
    float ao = texture(material.aoMap1, tex[material.aoMap1_uv]).r;
    float roughness = metallicRougness.g;
    float metallic  = metallicRougness.b;

    vec3 L0 = vec3(0.0f);
    for(int i = 0; i < 3; i++){
        vec3  lightDiff = lights[i].position.xyz - lights[i].position.w * worldPosition.xyz;
        vec3  lightDir  = normalize(lightDiff);
        float distance    = length(lightDiff) * lights[i].position.w;
        float attenuation = 1.0f / (distance * distance);
        vec3  radiance    = lights[i].emittance * attenuation;

        L0 += shadePunctual(normal, viewDir, lightDir, radiance, albedo, roughness, metallic);
    }

    //IBL environment lighting
    vec3 ambient = shadeIBL(normal, viewDir, albedo, roughness, metallic, ao);
    vec3 color = L0 + ambient;

    fragmentColor = vec4(color, 1.0f);
}
