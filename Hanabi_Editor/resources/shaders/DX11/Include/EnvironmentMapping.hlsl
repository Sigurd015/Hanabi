#ifndef ENVMAPPING_HEADER
#define ENVMAPPING_HEADER
#include "Common.hlsl"

float3 GetCubeMapTexCoord(uint3 ThreadID, float2 imageSize)
{
    float2 st = ThreadID.xy / imageSize;
    //float2 uv = 2.0 * float2(st.x, st.y) - float2(1.0f, 1.0f);
    float2 uv = 2.0 * float2(st.x, 1.0f - st.y) - float2(1.0f, 1.0f);

    float3 ret;
    if (ThreadID.z == 0)      ret = float3( 1.0f,  uv.y, -uv.x);
    else if (ThreadID.z == 1) ret = float3(-1.0f,  uv.y,  uv.x);
    else if (ThreadID.z == 2) ret = float3( uv.x,  1.0f, -uv.y);
    else if (ThreadID.z == 3) ret = float3( uv.x, -1.0f,  uv.y);
    else if (ThreadID.z == 4) ret = float3( uv.x,  uv.y,  1.0f);
    else if (ThreadID.z == 5) ret = float3(-uv.x,  uv.y, -1.0f);
    return normalize(ret);
}

#endif