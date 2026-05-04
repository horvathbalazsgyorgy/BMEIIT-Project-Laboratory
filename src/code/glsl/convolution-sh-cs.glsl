#version 430 core

precision highp float;

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout (std430, binding = 0) buffer SHGroupData{
    vec4  data[288];
    float area[32];
} outData;

shared float groupTemp[256 * 9];
shared float groupArea[256];

uniform samplerCube envTexture;

const float rows = 64.0f;
const float cols = 128.0f;
const float PI = 3.14159265359;

struct SHBase{
    float[9] values;
};

SHBase GetInit(){
    SHBase init;
    for(int i = 0; i < 9; i++){
        init.values[i] = 0.0f;
    }
    return init;
}

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

void WriteOutputSH(SHBase channel, uint tid, uint gid, uint num, uint index){
    for(int i = 0; i < 9; i++){
        groupTemp[tid * 9 + i] = channel.values[i];
    }

    barrier();
    for(uint s = num/2; s > 0; s >>= 1){
        if(tid < s){
            for(int i = 0; i < 9; i++){
                groupTemp[tid * 9 + i] += groupTemp[(tid + s) * 9 + i];
            }
        }
        barrier();
    }

    barrier();
    if(tid == 0){
        for(int i = 0; i < 9; i++){
            outData.data[gid * 9 + i][index] = groupTemp[i];
        }
    }
}

void WriteOutputArea(float weight, uint tid, uint gid, uint num){
    groupArea[tid] = weight;

    barrier();
    for(uint s = num/2; s > 0; s >>= 1){
        if(tid < s){
            groupArea[tid] += groupArea[tid + s];
        }
        barrier();
    }

    barrier();
    if(tid == 0){
        outData.area[gid] = groupArea[0];
    }
}

void SumWeighted(inout SHBase channel, SHBase base, float weight){
    for(int i = 0; i < 9; i++){
        channel.values[i] += base.values[i] * weight;
    }
}

void main(void) {
    uvec2 gtid = uvec2(gl_GlobalInvocationID.xy);
    uint   tid = gl_LocalInvocationIndex;
    uint   gid = gl_WorkGroupID.y   * gl_NumWorkGroups.x + gl_WorkGroupID.x;
    uint   num = gl_WorkGroupSize.x * gl_WorkGroupSize.y;

    SHBase red   = GetInit();
    SHBase green = GetInit();
    SHBase blue  = GetInit();

    float azimuthDelta = (2.0f * PI) / cols;
    float zenithDelta  = PI / rows;

    float azimuth = (float(gtid.x) / cols) * 2.0f * PI;
    float zenith  = (float(gtid.y) / rows) * PI;
    float weight  = sin(zenith) * azimuthDelta * zenithDelta;

    vec3 tangent = vec3(0.0f);
    tangent.x = sin(zenith) * cos(azimuth);
    tangent.y = sin(zenith) * sin(azimuth);
    tangent.z = cos(zenith);

    vec3 radiance = texture(envTexture, tangent).rgb;
    if(any(isnan(radiance)) || any(isinf(radiance))){
        radiance = vec3(0.0f);
    }

    SHBase base = GetBase(tangent);

    SumWeighted(red,   base, weight * radiance.x);
    SumWeighted(green, base, weight * radiance.y);
    SumWeighted(blue,  base, weight * radiance.z);

    WriteOutputSH(red,   tid, gid, num, 0); barrier();
    WriteOutputSH(green, tid, gid, num, 1); barrier();
    WriteOutputSH(blue,  tid, gid, num, 2); barrier();
    WriteOutputArea(weight, tid, gid, num);
}
