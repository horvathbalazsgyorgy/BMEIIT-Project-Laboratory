#version 430 core

layout (local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) buffer SHGroupData{
    vec4  data[288];
    float area[32];
} inData;

layout (std430, binding = 1) buffer SHData{
    vec4 radiance[9];
} finalData;

shared float groupTemp[288];
shared float groupArea[32];

struct SHBase{ float[9] values; };

const float PI = 3.14159265359;
const float normFactor = 1.0f;

const float SHCosine0 = PI;
const float SHCosine1 = (2.0f * PI) / 3.0f;
const float SHCosine2 = PI * 0.25f;

const float SHConstant1PI4   = 0.282095f;
const float SHConstant3PI4   = 0.488603f;
const float SHConstant15PI4  = 1.092548f;
const float SHConstant5PI16  = 0.315339f;
const float SHConstant15PI16 = 0.546274f;

SHBase GetCoefficients(){
    SHBase base;
    base.values[0] = SHConstant1PI4   * SHCosine0 * normFactor;
    base.values[1] = SHConstant3PI4   * SHCosine1 * normFactor;
    base.values[2] = SHConstant3PI4   * SHCosine1 * normFactor;
    base.values[3] = SHConstant3PI4   * SHCosine1 * normFactor;
    base.values[4] = SHConstant15PI4  * SHCosine2 * normFactor;
    base.values[5] = SHConstant15PI4  * SHCosine2 * normFactor;
    base.values[6] = SHConstant5PI16  * SHCosine2 * normFactor;
    base.values[7] = SHConstant15PI4  * SHCosine2 * normFactor;
    base.values[8] = SHConstant15PI16 * SHCosine2 * normFactor;
    return base;
}

void main(void) {
    uint   tid = gl_LocalInvocationIndex;
    uint   num = gl_WorkGroupSize.x * gl_WorkGroupSize.y;

    groupArea[tid] = inData.area[tid];
    barrier();
    for(uint s = num/2; s > 0; s >>= 1){
        if(tid < s){
            groupArea[tid] += groupArea[tid + s];
        }
        barrier();
    }
    barrier();

    for(int j = 0; j < 3; j++){
        for(uint i = 0u; i < 9u; i++){
            groupTemp[tid * 9 + i] = inData.data[tid * 9 + i][j];
        }
        barrier();

        for(uint s = num/2; s > 0; s >>= 1){
            if(tid < s){
                for(uint k = 0u; k < 9u; k++){
                    groupTemp[tid * 9 + k] += groupTemp[(tid + s) * 9 + k];
                }
            }
            barrier();
        }
        barrier();

        if(tid == 0){
            SHBase base = GetCoefficients();
            for(uint i = 0u; i < 9u; i++){
                groupTemp[i] *= (1.0f / groupArea[0]);
                groupTemp[i] *= base.values[i];
                finalData.radiance[i][j] = groupTemp[i];
            }
        }
        barrier();
    }
}
