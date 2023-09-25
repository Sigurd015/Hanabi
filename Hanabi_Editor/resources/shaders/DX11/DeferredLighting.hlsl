// --------------------------
// Deferred Lighting Shader
// --------------------------

#type:vertex
#include "FullScreenQuadVertex.hlsl"

#type:pixel
#include "Buffers.hlsl"
#include "Lighting.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float2 TexCoord : Tex;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D u_DiffuseBuffer : register(t3);
Texture2D u_SpecularBuffer : register(t4);
Texture2D u_NormalBuffer : register(t5);
Texture2D u_PositionBuffer : register(t6);

SamplerState u_SSLinearWrap : register(s0);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    Material material;
    float2 texCoord = float2(Input.TexCoord.x, 1.0 - Input.TexCoord.y);
    material.DiffuseColor = u_DiffuseBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    material.SpecularColor = u_SpecularBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    material.WorldNormal = u_NormalBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    material.WorldPosition = u_PositionBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    material.PixelToCamera = normalize(u_CameraPosition - material.WorldPosition);

    float3 ambient = u_SkyLightIntensity * float3(1.0f, 1.0f, 1.0f) * material.DiffuseColor;

    float3 dirLightResult = CalcDirectionalLight(material);
    float3 pointLightResult = CalcPointLight(material);
    float3 spotLightResult = CalcSpotLight(material);
    Output.Color = float4(saturate(ambient + dirLightResult + pointLightResult + spotLightResult), 1.0f);
    return Output;
}