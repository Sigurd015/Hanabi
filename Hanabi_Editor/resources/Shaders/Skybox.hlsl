// --------------------------
// Skybox Shader
// --------------------------

#type:vertex
#include "Buffers.hlsl"

struct VertexInput
{
    float3 a_Position : a_Position;
};

struct VertexOutput
{
    float4 Pos : SV_Position;
    float3 WorldPos : WorldPos;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.Pos = mul(u_ViewProjection, float4(Input.a_Position, 0.0f));
    Output.Pos.z = Output.Pos.w;
    Output.WorldPos = Input.a_Position;
    return Output;
}

#type:pixel
struct PixelInput
{
    float4 Pos : SV_Position;
    float3 WorldPos : WorldPos;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

TextureCube u_RadianceMap : register(t5);
SamplerState u_SSLinearWrap : register(s0);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    Output.Color = u_RadianceMap.Sample(u_SSLinearWrap, Input.WorldPos);
    return Output;
}