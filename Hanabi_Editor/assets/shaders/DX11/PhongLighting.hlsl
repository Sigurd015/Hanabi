// --------------------------
// Phong Lighting Shader
// --------------------------

#type:vertex
#include "Buffers.hlsl"

struct VertexInput
{
    float3 a_Position : a_Position;
    float3 a_Normal : a_Normal;
    float2 a_TexCoord : a_TexCoord;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 WorldPosition : WorldPos;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float3 CameraPosition : CamPos;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.WorldPosition = mul(u_Transform, float4(Input.a_Position, 1.0f));
    Output.Position = mul(u_ViewProjection, float4(Output.WorldPosition, 1.0));
    
    Output.Normal = mul((float3x3) (u_Transform), Input.a_Normal);
    Output.TexCoord = Input.a_TexCoord;
    Output.CameraPosition = u_CameraPosition;
    
    return Output;
}

#type:pixel
#include "Lighting.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float3 WorldPosition : WorldPos;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float3 CameraPosition : CamPos;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D u_Diffuse : register(t0);
SamplerState u_DiffuseSamplerState : register(s0);

Texture2D u_Specular : register(t1);
SamplerState u_SpecularSamplerState : register(s1);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    Material material;
    material.DiffuseColor = u_Diffuse.Sample(u_DiffuseSamplerState, Input.TexCoord);
    material.SpecularColor = u_Specular.Sample(u_SpecularSamplerState, Input.TexCoord);
    
    float3 PixelToCamera = normalize(Input.CameraPosition - Input.WorldPosition);
    Input.Normal = normalize(Input.Normal);
    float3 dirLightResult = CalcDirectionalLight(material, Input.Normal, PixelToCamera);
    float3 pointLightResult = CalcPointLight(material, Input.Normal, PixelToCamera, Input.WorldPosition);
    float3 spotLightResult = CalcSpotLight(material, Input.Normal, PixelToCamera, Input.WorldPosition);
    Output.Color = float4(saturate(dirLightResult + pointLightResult + spotLightResult), 1.0f);
    return Output;
}