#version 430 core

precision highp float;

layout (std430, binding = 1) buffer SHData{
    vec4 radiance[9];
} SHColor;

in vec2 tex;

uniform struct{
    sampler2D gPosition;
    sampler2D gNormal;
    sampler2D gAlbedo;
    sampler2D gPBR;
} material;

uniform struct{
    vec3 position;
} camera;

uniform struct{
    vec4 position;
    vec3 emittance;
} lights[4];

uniform int   spherical;
uniform float exposure;
uniform float LoD;
uniform sampler2D LuT;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;

out vec4 fragmentColor;
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

//Courtesy of Krzysztof Narkowicz
//https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACEScurve(vec3 x){
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp(vec3((x * (a*x+b)) / (x * (c*x+d) + e)), 0.0, 1.0);
}

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

    //Fresnel
    vec3 F  = FresnelRoughness(F0, cosb, roughness);
    vec3 ks = F;
    vec3 kd = vec3(1.0f) - ks;
    kd *= 1.0f - metallic;

    vec3 diffuse = vec3(0.0f);
    if(spherical == 0){
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
    vec3 reflection     = reflect(-viewDir, normal);
    vec3 prefilterColor = textureLod(prefilterMap, reflection, roughness * LoD).rgb;
    vec2 envBRDF  = texture(LuT, vec2(cosb, roughness)).rg;
    vec3 specular = prefilterColor * (F * envBRDF.x + envBRDF.y);

    return (kd * diffuse + specular) * ao;
}

void main(void) {
    //Extracting data from G-Buffer
    vec3  position  = texture(material.gPosition, tex).rgb;
    vec3  normal    = texture(material.gNormal,   tex).rgb;
    vec3  albedo    = texture(material.gAlbedo,   tex).rgb;
    vec3  pbr       = texture(material.gPBR,      tex).rgb;
    float ao        = pbr.r;
    float roughness = pbr.g;
    float metallic  = pbr.b;
    normal = normalize(normal);

    vec3 viewDir = normalize(camera.position - position);

    vec3 L0 = vec3(0.0f);
    for(int i = 0; i < 4; i++){
        vec3  lightDiff = lights[i].position.xyz - lights[i].position.w * position;
        vec3  lightDir  = normalize(lightDiff);
        float distance    = length(lightDiff) * lights[i].position.w;
        float attenuation = 1.0f / (distance * distance);
        vec3  radiance    = lights[i].emittance * attenuation;

        L0 += shadePunctual(normal, viewDir, lightDir, radiance, albedo, roughness, metallic);
    }

    //IBL environment lighting
    vec3 ambient = shadeIBL(normal, viewDir, albedo, roughness, metallic, ao);
    vec3 color = L0 + ambient;

    color *= exposure;
    vec3 toneMapped = ACEScurve(color);
    fragmentColor = vec4(toneMapped, 1.0f);
}
