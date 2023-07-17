// --------------------------
// Phong Lighting Shader
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
    Output.Tangent = Input.a_Tangent;
    Output.Bitangent = Input.a_Bitangent;
    Output.Normal = Input.a_Normal;

    return Output;
}

#type:pixel
#include "Buffers.hlsl"
#include "Lighting.hlsl"
#include "Textures.hlsl"

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
    float4 Color : SV_Target0;
};

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    Material material;
    material.DiffuseColor = u_Diffuse.Sample(u_DiffuseSamplerState, Input.TexCoord).xyz;
    material.SpecularColor = u_Specular.Sample(u_SpecularSamplerState, Input.TexCoord).xyz;
      
    Input.Normal = normalize(mul((float3x3) (u_Transform), Input.Normal));
    
    float3 normal = float3(0, 0, 0);
    if (u_UseNormalMap)
    {
        Input.Tangent = normalize(mul((float3x3) (u_Transform), Input.Tangent));
        Input.Bitangent = normalize(mul((float3x3) (u_Transform), Input.Bitangent));
        float3x3 mat = float3x3(Input.Tangent, Input.Bitangent, Input.Normal);

        float3 normalSample = u_Normal.Sample(u_NormalSamplerState, Input.TexCoord).xyz;
        normal = normalSample * 2.0f - 1.0f; // from RGB[0, 1] to [-1, 1]
        normal = normalize(mul(mat, normal));
    }
    else
        normal = Input.Normal;
    
    float3 PixelToCamera = normalize(u_CameraPosition - Input.WorldPosition);
    float3 dirLightResult = CalcDirectionalLight(material, normal, PixelToCamera);
    float3 pointLightResult = CalcPointLight(material, normal, PixelToCamera, Input.WorldPosition);
    float3 spotLightResult = CalcSpotLight(material, normal, PixelToCamera, Input.WorldPosition);
    Output.Color = float4(saturate(dirLightResult + pointLightResult + spotLightResult), 1.0f);
    return Output;
}