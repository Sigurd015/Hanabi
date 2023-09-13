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
    float4 LightTransformedPosition : LightTransformedPosition;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.WorldPosition = mul(u_Transform, float4(Input.a_Position, 1.0f));
    Output.Position = mul(u_ViewProjection, float4(Output.WorldPosition, 1.0));
    Output.LightTransformedPosition = mul(u_LightViewProjection, float4(Output.WorldPosition, 1.0));
    Output.TexCoord = Input.a_TexCoord;
    Output.Tangent = mul((float3x3) (u_Transform), Input.a_Tangent);
    Output.Bitangent = mul((float3x3) (u_Transform), Input.a_Bitangent);
    Output.Normal = mul((float3x3) (u_Transform), Input.a_Normal);

    return Output;
}

#type:pixel
#include "Buffers.hlsl"
#include "Lighting.hlsl"
#include "Material.hlsl"
#include "ShadowMapping.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float3 WorldPosition : WorldPos;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float3 Tangent : Tangent;
    float3 Bitangent : Bitangent;
    float4 LightTransformedPosition : LightTransformedPosition;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    Material material;
    material.DiffuseColor = u_Diffuse.Sample(u_SSLinearWrap, Input.TexCoord).xyz;
    material.SpecularColor = u_Specular.Sample(u_SSLinearWrap, Input.TexCoord).xyz;

    material.WorldNormal = normalize(Input.Normal);
    if (u_UseNormalMap)
    {
        float3x3 mat = float3x3(normalize(Input.Tangent), normalize(Input.Bitangent), material.WorldNormal);

        float3 normalSample = u_Normal.Sample(u_SSLinearWrap, Input.TexCoord).xyz;
        material.WorldNormal = normalSample * 2.0f - 1.0f; // from RGB[0, 1] to [-1, 1]
        material.WorldNormal = normalize(mul(mat, material.WorldNormal));
    }

    float shadowResult = 1.0f;
    switch (u_ShadowType)
    {
        case 1:
            shadowResult = CalculateHardShadow(Input.LightTransformedPosition, material.WorldNormal);
            break;
        case 2:
            shadowResult = CalculateSoftShadow(Input.LightTransformedPosition, material.WorldNormal);
            break;
    }

    float3 ambient = u_SkyLightIntensity * float3(1.0f, 1.0f, 1.0f) * material.DiffuseColor;

    float3 PixelToCamera = normalize(u_CameraPosition - Input.WorldPosition);
    float3 dirLightResult = CalcDirectionalLight(material, PixelToCamera);
    float3 pointLightResult = CalcPointLight(material, PixelToCamera, Input.WorldPosition);
    float3 spotLightResult = CalcSpotLight(material, PixelToCamera, Input.WorldPosition);
    float3 lightResult = saturate(dirLightResult + pointLightResult + spotLightResult) * shadowResult;
    Output.Color = float4(saturate(ambient + lightResult), 1.0f);
    return Output;
}