// --------------------------
// DirShadow Map Shader
// --------------------------

#type:vertex
#include "Buffers.hlsl"

struct VertexInput
{
    float3 a_Position : a_Position;
    float3 a_Normal : a_Normal;
    float3 a_Tangent : a_Tangent;
    float3 a_Bitangent : a_Bitangent;
    float2 a_TexCoord : a_TexCoord;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 TexCoord : TexCoord;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    float3 worldPos = mul(u_Transform, float4(Input.a_Position, 1.0f));
    Output.Position = mul(u_DirLightViewProjection, float4(worldPos, 1.0));
    Output.TexCoord = Input.a_TexCoord;
    return Output;
}

#type:pixel
#include "Common.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float2 TexCoord : TexCoord;
};

Texture2D u_AlbedoTex : register(t0);

void main(PixelInput Input)
{
    float4 albedo = u_AlbedoTex.Sample(u_SSAnisotropicWrap, Input.TexCoord);
    // Discard transparent pixels
    clip(albedo.a - 0.01f);
}