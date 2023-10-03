// --------------------------
// Deferred Lighting Shader
// --------------------------

#type:vertex
#include "FullScreenQuadVertex.hlsl"

#type:pixel
#include "Buffers.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float2 TexCoord : Tex;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D u_AlbedoBuffer : register(t3);
Texture2D u_MetalnessRoughnessBuffer : register(t4);
Texture2D u_NormalBuffer : register(t5);
Texture2D u_PositionBuffer : register(t6);

SamplerState u_SSLinearWrap : register(s0);

PixelOutput main(PixelInput Input)
{
    PixelOutput output;
    //PBRParameters params;
    float2 texCoord = float2(Input.TexCoord.x, 1.0 - Input.TexCoord.y);
    //params.DiffuseColor = u_DiffuseBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    //params.SpecularColor = u_SpecularBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    //params.WorldNormal = u_NormalBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    //params.WorldPosition = u_PositionBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    //params.PixelToCamera = normalize(u_CameraPosition - material.WorldPosition);

    //float3 ambient = u_SkyLightIntensity * float3(1.0f, 1.0f, 1.0f) * material.DiffuseColor;

    //float3 dirLightResult = CalcDirectionalLight(params);
    //float3 pointLightResult = CalcPointLight(params);
    //float3 spotLightResult = CalcSpotLight(params);
    //Output.Color = float4(saturate(ambient + dirLightResult + pointLightResult + spotLightResult), 1.0f);
    output.Color = u_AlbedoBuffer.Sample(u_SSLinearWrap, texCoord);
    return output;
}