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
    float3x3 TBN : TBN;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    float4 worldPosition = mul(u_Transform, float4(Input.a_Position, 1.0f));
    Output.WorldPosition = worldPosition.xyz;  
    Output.TexCoord = Input.a_TexCoord;
    Output.Normal = mul((float3x3)u_Transform, Input.a_Normal);
    Output.TBN = mul((float3x3)u_Transform, transpose(float3x3(Input.a_Tangent, Input.a_Bitangent, Input.a_Normal)));
    Output.Position = mul(u_ViewProjection, worldPosition);
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
    float3x3 TBN : TBN;
};

struct PixelOutput
{
    float4 Albedo : SV_Target0;
    float4 MRE : SV_Target1;  // Metalness, Roughness, Emission
    float4 Normal : SV_Target2;
    float4 Position : SV_Target3;
};

Texture2D u_AlbedoTex : register(t0);
Texture2D u_MetalnessTex : register(t1);
Texture2D u_RoughnessTex : register(t2);
Texture2D u_NormalTex : register(t3);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    float4 albedo = u_AlbedoTex.Sample(u_SSAnisotropicWrap, Input.TexCoord);
    float metalness = u_MetalnessTex.Sample(u_SSLinearWrap, Input.TexCoord).r * u_Material.Metalness;
    float roughness = u_RoughnessTex.Sample(u_SSLinearWrap, Input.TexCoord).r * u_Material.Roughness;

    float3 normal = normalize(Input.Normal);
    if (u_Material.UseNormalMap)
    {
        normal = normalize(u_NormalTex.Sample(u_SSLinearWrap, Input.TexCoord).rgb * 2.0f - 1.0f); // from RGB[0, 1] to [-1, 1]
        normal = normalize(mul(Input.TBN, normal));
    }

    // Texture is created in UNORM format, so we need to convert it to linear space
    albedo.rgb = SRGBToLinear(albedo.rgb, 2.2f);
    Output.Albedo = float4(albedo.rgb * u_Material.Albedo, albedo.a);
    // Minimum roughness of 0.05 to keep specular highlight
    Output.MRE = float4(metalness, max(roughness, 0.05f), u_Material.Emission, 1.0f);
    Output.Normal = float4(normal, 1.0f);
    Output.Position = float4(Input.WorldPosition, 1.0f);

    return Output;
}