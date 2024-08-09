// --------------------------
// Composite Shader
// --------------------------

#pragma stage : compute
#include "Include/Common.hlsl"

Texture2D u_InputBuffer : register(t0);
RWTexture2D<float4> u_OutputBuffer : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 ThreadID : SV_DispatchThreadID)
{
    float4 color = u_InputBuffer.Load(uint3(ThreadID.xy, 0));
    u_OutputBuffer[ThreadID.xy] = float4(GammaCorrect(ACESTonemap(color.xyz), 2.2f), 1.0f);
}