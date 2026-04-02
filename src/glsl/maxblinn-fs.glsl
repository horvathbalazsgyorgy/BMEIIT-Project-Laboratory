#version 430 core

precision highp float;

in vec4 worldPosition;
in vec4 worldNormal;
in vec4 worldTangent;
in vec4 worldBitangent;
in vec3 viewDir;
in vec2 tex;

uniform struct{
    sampler2D diffuseMap1;
    sampler2D normalMap1;
} material;

uniform struct{
    vec4 position;
    vec3 emittance;
    vec3 ambient;
} lights[3];

out vec4 fragmentColor;

vec4 shade(vec3 normal, vec3 lightDir, vec3 emittance, vec3 ambient, float distance){
    vec3 kd = texture(material.diffuseMap1, tex).rgb;
    vec3 ks = vec3(1.0f, 1.0f, 1.0f);

    vec3 ambientColor = ambient * kd;

    float cosa = max(dot(normal, lightDir), 0.0f);
    vec3 diffuseColor = emittance * kd * cosa;

    vec3 halfway = normalize(lightDir + viewDir);
    float cosb   = max(dot(normal, viewDir), 0.0f);
    float cost   = pow(max(dot(normal, halfway), 0.0f), 64.0f);
    vec3 specularColor = emittance * ks * cost * (cosa / max(cosb, cosa));

    float attenuation = 1.0f / (1.0f + 0.07 * distance + 0.017 * (distance * distance));
    ambientColor  *= attenuation;
    diffuseColor  *= attenuation;
    specularColor *= attenuation;

    return vec4(ambientColor + diffuseColor + specularColor, 1.0f);
}

void main(void) {
    fragmentColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    vec3 N = normalize(worldNormal.xyz);
    vec3 T = normalize(worldTangent.xyz);
    vec3 B = normalize(worldBitangent.xyz);

    mat3 TBN = mat3(T, B, N);

    vec3 tangentNormal = texture(material.normalMap1, tex).rgb;
    tangentNormal = normalize(tangentNormal * 2.0 - 1.0);

    vec3 normal = normalize(TBN * tangentNormal);

    for(int i = 0; i < lights.length(); i++){
        vec3 lightDiff = lights[i].position.xyz - lights[i].position.w * worldPosition.xyz;
        vec3 lightDir  = normalize(lightDiff);
        float distance = length(lightDiff) * lights[i].position.w;

        fragmentColor += shade(normal, lightDir, lights[i].emittance, lights[i].ambient, distance);
    }
}
