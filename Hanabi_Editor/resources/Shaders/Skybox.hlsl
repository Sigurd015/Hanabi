// --------------------------
// Skybox Shader
// --------------------------

#pragma stage : vertex
#include "Include/Buffers.hlsl"

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

#pragma stage : pixel
#include "Include/Common.hlsl"

struct PixelInput
{
    float4 Pos : SV_Position;
    float3 WorldPos : WorldPos;
};

TextureCube u_RadianceMap : register(t5);

float4 main(PixelInput Input) : SV_Target
{
    // HDR image may in linear space by default
    return float4(GammaCorrect(ACESTonemap(u_RadianceMap.Sample(u_SSLinearWrap, Input.WorldPos).xyz), 2.2f), 1.0f);
}