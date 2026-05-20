#version 430 core

precision highp float;

in vec2 tex;

uniform struct{
    sampler2D gPosition;
    sampler2D gNormal;
    sampler2D gAlbedo;
} material;

uniform struct{
    vec3 position;
} camera;

uniform struct{
    vec4 position;
    vec3 emittance;
    vec3 ambient;
} lights[4];

out vec4 fragmentColor;

vec4 shade(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, vec3 emittance, vec3 ambient, float distance){
    vec3 kd = albedo;
    vec3 ks = vec3(1.0f, 1.0f, 1.0f);

    vec3 ambientColor = ambient * kd;

    float cosa = max(dot(normal, lightDir), 0.0f);
    vec3 diffuseColor = emittance * kd * cosa;

    vec3 halfway = normalize(lightDir + viewDir);
    float cosb   = max(dot(normal, viewDir), 0.0f);
    float cost   = pow(max(dot(normal, halfway), 0.0f), 64.0f);
    vec3 specularColor = emittance * ks * cost * (cosa / max(cosb, cosa));

    //float attenuation = 1.0f / (1.0f + 0.07 * distance + 0.017 * (distance * distance));
    float attenuation = 1.0f / (distance * distance);
    diffuseColor  *= attenuation;
    specularColor *= attenuation;

    return vec4(ambientColor + diffuseColor + specularColor, 1.0f);
}

void main(void) {
    fragmentColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    vec3 position  = texture(material.gPosition, tex).rgb;
    vec3 normal    = texture(material.gNormal,   tex).rgb;
    vec3 albedo    = texture(material.gAlbedo,   tex).rgb;
    normal = normalize(normal);

    vec3 viewDir   = normalize(camera.position - position);

    for (int i = 0; i < 4; i++){
        vec3 lightDiff = lights[i].position.xyz - lights[i].position.w * position;
        vec3 lightDir  = normalize(lightDiff);
        float distance = length(lightDiff);

        fragmentColor += shade(normal, lightDir, viewDir, albedo, lights[i].emittance, lights[i].ambient, distance);
    }
    fragmentColor = clamp(fragmentColor, 0.0, 1.0);
}
