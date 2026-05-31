#version 430 core

precision highp float;

layout (std430, binding = 1) buffer SHData{
    vec4 radiance[9];
} SHColor;

in vec4 viewPosition;
in vec2 tex;

uniform struct{
    sampler2D gNormal;
    sampler2D gAlbedo;
    sampler2D gPBR;
    sampler2D gEmissive;
} material;

uniform struct {
    mat4 view;
    mat4 projection;
    mat4 invView;
    mat4 invProjection;
    vec3 position;
} camera;

uniform struct{
    vec4 position;
    vec3 emittance;
} lights[6];

uniform int   PBRold;
uniform int   spherical;
uniform float exposure;
uniform float LoD;
uniform sampler2D LuT;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D SSAO;
uniform int ambient;

layout (location = 0) out vec3 fragmentColor;
const float PI = 3.14159265359;

struct SHBase{
    float[9] values;
};

SHBase GetBase(vec3 dir){
    SHBase base;
    base.values[0] = 1.0f;
    base.values[1] = dir.y;
    base.values[2] = dir.z;
    base.values[3] = dir.x;
    base.values[4] = dir.x * dir.y;
    base.values[5] = dir.y * dir.z;
    base.values[6] = (3.0f * dir.z*dir.z - 1.0f);
    base.values[7] = dir.x * dir.z;
    base.values[8] = (dir.x*dir.x - dir.y*dir.y);
    return base;
}

//Normal Distribution function - Using the Beckmann approximation
float Distribution_Beckmann(float roughness, float cost){
    float a     = max(roughness * roughness, 1e-3);
    float a2    = a * a;
    float cos0  = max(cost * cost, 1e-3);
    float cos00 = cos0 * cos0;
    float exponent = 1.0f/a2 * (1.0f - 1.0f/cos0);
    float nom   = 1.0f/PI * 1.0f/a2 * 1.0f/cos00;

    return nom * exp(exponent);
}

//Normal Distribution function - Using the Trowbridge-Reitz GGX
float Distribution_Trowbridge_Reitz_GGX(float roughness, float cost){
    float a     = max(roughness * roughness, 1e-3);
    float a2    = a * a;
    float cos0  = max(cost * cost, 1e-3);
    float term  = cos0 * (a2 - 1.0f) + 1.0f;
    float denom = PI * (term * term);

    return a2 / denom;
}

//Geometry function - Using the Kelemen-Szirmay approximation
float Geometry_Kelemen(float cosa, float cosb, float cost){
    float cos0 = max(cost * cost, 1e-3);
    float invCos0 = 1.0f / cos0;

    return cosa * cosb * invCos0;
}

//Geometry function - Using the Schlick-GGX approximation
float Geometry_Schlick_GGX(float k, float cosa, float cosb){
    float vGGX = cosb / max(cosb * (1.0f - k) + k, 1e-3);
    float lGGX = cosa / max(cosa * (1.0f - k) + k, 1e-3);

    return vGGX * lGGX;
}

//Fresnel function - Using the Fresnek-Schlick approximation
vec3 Fresnel_Schlick(vec3 F0, float cost){
    return F0 + (1.0f - F0) * pow(clamp(1.0f - cost, 0.0f, 1.0f), 5.0);
}

//Fresnel function - Using the Fresnek-Schlick approximation
vec3 FresnelRoughness(vec3 F0, float cost, float roughness){
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(clamp(1.0f - cost, 0.0f, 1.0f), 5.0);
}

vec3 CookTorranceBRDF(float ND, float G, vec3 F, float cosa, float cosb){
    vec3   DGF   = ND * G * F;
    float  denom = max(4.0f * cosa * cosb, 1e-4);
    return DGF / denom;
}

vec3 shadePunctual(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 radiance, vec3 albedo, float roughness, float metallic){
    //Angles
    vec3 halfway = normalize(lightDir + viewDir);
    float cosa = max(dot(normal, lightDir), 0.0f);
    float cosb = max(dot(normal, viewDir),  0.0f);
    float cost = max(dot(normal, halfway),  0.0f);
    float cos0 = max(dot(halfway, viewDir), 0.0f);

    //Fresnel component
    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);
    vec3 F = Fresnel_Schlick(F0, cos0);

    vec3 specularColor = vec3(0.0f);
    if(PBRold == 1){
        float G = Geometry_Kelemen(cosa, cosb, cos0);
        float ND = Distribution_Beckmann(roughness, cost);

        specularColor = ND * G * F * 0.25f;
    }else{
        float r = roughness + 1.0f;
        float k = (r * r) / 8.0f;
        float G = Geometry_Schlick_GGX(k, cosa, cosb);
        float ND = Distribution_Trowbridge_Reitz_GGX(roughness, cost);

        specularColor = CookTorranceBRDF(ND, G, F, cosa, cosb);
    }

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

    //Fresnel
    vec3 F  = FresnelRoughness(F0, cosb, roughness);
    vec3 ks = F;
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic;

    vec3 diffuse = vec3(0.0f);
    if(spherical == 1){
        //Irradiance using Spherical Harmonics
        SHBase normalBase = GetBase(normal);
        vec3 irradiance = vec3(0.0f);
        for(int i = 0; i < 9; i++){
            irradiance += SHColor.radiance[i].xyz * normalBase.values[i];
        }
        diffuse = max(irradiance, 0.0) * albedo * (1.0f / PI);
    }else{
        //Irradiance using sanitized irradiance map
        vec3 irradiance = texture(irradianceMap, normal).rgb;
        diffuse = irradiance * albedo;
    }

    //Specular using sanitized pre-filter map
    vec3 reflection     = normalize(reflect(-viewDir, normal));
    vec3 prefilterColor = textureLod(prefilterMap, reflection, roughness * LoD).rgb;
    vec2 envBRDF  = texture(LuT, vec2(cosb, roughness)).rg;
    vec3 specular = prefilterColor * (F * envBRDF.x + envBRDF.y);

    return (kd * diffuse + specular) * ao;
}

void main(void) {
    //Extracting data from G-Buffer
    vec4  Locations = texture(material.gNormal, tex);

    vec3  viewRay  = viewPosition.xyz;
    float depth    = Locations.w;
    vec3  viewPos  = viewRay * depth;
    vec4  worldPos = camera.invView * vec4(viewPos, 1.0f);
    vec3  position = worldPos.xyz / worldPos.w;

    vec3  normal    = normalize(Locations.xyz);
    vec3  albedo    = texture(material.gAlbedo,   tex).rgb;
    vec3  pbr       = texture(material.gPBR,      tex).rgb;
    vec3  emission  = texture(material.gEmissive, tex).rgb;
    float occlusion = texture(SSAO, tex).r;
    float roughness = pbr.r;
    float metallic  = pbr.g;

    if(ambient == 0){
        occlusion = 1.0f;
    }

    vec3 viewDir = normalize(camera.position - position);

    vec3 L0 = vec3(0.0f);
    for(int i = 0; i < 6; i++){
        vec3  lightDiff = lights[i].position.xyz - lights[i].position.w * position;
        vec3  lightDir  = normalize(lightDiff);
        float distance    = length(lightDiff) * lights[i].position.w;
        float attenuation = 1.0f / (distance * distance);
        vec3  radiance    = lights[i].emittance * attenuation;

        L0 += shadePunctual(normal, viewDir, lightDir, radiance, albedo, roughness, metallic);
    }

    //IBL environment lighting
    vec3 ambient = shadeIBL(normal, viewDir, albedo, roughness, metallic, occlusion);
    vec3 color = L0 + ambient;
    color += emission;
    fragmentColor = color;
}
