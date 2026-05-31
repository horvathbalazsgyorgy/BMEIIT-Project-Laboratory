#version 430 core

precision highp float;

in vec4 viewPosition;
in vec2 tex;

uniform struct{
    sampler2D gNormal;
    sampler2D gAlbedo;
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

uniform int ambient;
uniform sampler2D SSAO;

layout (location = 0) out vec3 fragmentColor;

vec3 shade(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 albedo, vec3 emittance, float ambient, float distance){
    vec3 kd = albedo;
    vec3 ks = vec3(1.0f, 1.0f, 1.0f);

    vec3 ambientColor = 0.05 * ambient * kd;

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

    return vec3(ambientColor + diffuseColor + specularColor);
}

void main(void) {
    fragmentColor = vec3(0.0f, 0.0f, 0.0f);

    //Extracting data from G-Buffer
    vec4  Locations = texture(material.gNormal, tex);

    vec3  viewRay  = viewPosition.xyz;
    float depth    = Locations.w;
    vec3  viewPos  = viewRay * depth;
    vec4  worldPos = camera.invView * vec4(viewPos, 1.0f);
    vec3  position = worldPos.xyz / worldPos.w;

    vec3  normal    = normalize(Locations.xyz);
    vec3  albedo    = texture(material.gAlbedo, tex).rgb;
    float occlusion = texture(SSAO, tex).r;

    vec3  viewDir  = normalize(camera.position - position);

    if(ambient == 0){
        occlusion = 1.0f;
    }

    for (int i = 0; i < 6; i++){
        vec3 lightDiff = lights[i].position.xyz - lights[i].position.w * position.xyz;
        vec3 lightDir  = normalize(lightDiff);
        float distance = length(lightDiff);

        fragmentColor += shade(normal, lightDir, viewDir, albedo, lights[i].emittance, occlusion, distance);
    }
    fragmentColor = clamp(fragmentColor, 0.0, 1.0);
}
