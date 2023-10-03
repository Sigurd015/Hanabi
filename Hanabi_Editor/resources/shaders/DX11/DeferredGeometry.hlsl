// --------------------------
// Deferred Geometry Shader
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
    float3 WorldPosition : WorldPos;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float3 Tangent : Tangent;
    float3 Bitangent : Bitangent;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.WorldPosition = mul(u_Transform, float4(Input.a_Position, 1.0f));
    Output.Position = mul(u_ViewProjection, float4(Output.WorldPosition, 1.0));
    Output.TexCoord = Input.a_TexCoord;
    Output.Tangent = mul(u_Transform, float4(Input.a_Tangent, 0.0f)).xyz;
    Output.Bitangent = mul(u_Transform, float4(Input.a_Bitangent, 0.0f)).xyz;
    Output.Normal = mul(u_Transform, float4(Input.a_Normal, 0.0f)).xyz;

    return Output;
}

#type:pixel
#include "Buffers.hlsl"
#include "Common.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float3 WorldPosition : WorldPos;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float3 Tangent : Tangent;
    float3 Bitangent : Bitangent;
};

struct PixelOutput
{
    float4 Albedo : SV_Target0;
    float4 MetalnessRoughness : SV_Target1;
    float4 Normal : SV_Target2;
    float4 Position : SV_Target3;
};

Texture2D u_AlbedoTex : register(t0);
Texture2D u_MetalnessTex : register(t1);
Texture2D u_RoughnessTex : register(t2);
Texture2D u_NormalTex : register(t3);

SamplerState u_SSLinearWrap : register(s0);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    float4 albedo = u_AlbedoTex.Sample(u_SSLinearWrap, Input.TexCoord);
    float metalness = u_MetalnessTex.Sample(u_SSLinearWrap, Input.TexCoord).x * u_Material.Metalness;
    float roughness = u_RoughnessTex.Sample(u_SSLinearWrap, Input.TexCoord).x * u_Material.Roughness;

    float3 normal = normalize(Input.Normal);
    if (u_Material.UseNormalMap)
    {
        float3x3 mat = float3x3(normalize(Input.Tangent), normalize(Input.Bitangent), normal);

        float3 normalSample = u_NormalTex.Sample(u_SSLinearWrap, Input.TexCoord).xyz;
        normal = normalSample * 2.0f - float3(1.0f, 1.0f, 1.0f);; // from RGB[0, 1] to [-1, 1]
        normal = normalize(mul(mat, normal));
    }

    Output.Albedo = float4(SRGBToLinear(albedo.xyz), albedo.w);
    Output.MetalnessRoughness = float4(metalness, roughness, 0.0f, 0.0f);
    Output.Normal = float4(normal, 1.0f);
    Output.Position = float4(Input.WorldPosition, 1.0f);

    return Output;
}

