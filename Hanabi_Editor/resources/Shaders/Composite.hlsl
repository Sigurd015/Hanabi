// --------------------------
// Composite Shader
// --------------------------

#type:vertex
#include "FullScreenQuadVertex.hlsl"

#type:pixel
#include "Common.hlsl"

Texture2D u_Color : register(t0);

float4 main(float4 Position : SV_Position) : SV_Target
{
    float3 color = u_Color.Load(uint3(Position.xy, 0)).rgb;
    // TODO: Backbuffer is UNORM, so i think we dont need to gamma correct here
    //return float4(GammaCorrect(color, 2.2f), 1.0f);
    return float4(color, 1.0f);
}