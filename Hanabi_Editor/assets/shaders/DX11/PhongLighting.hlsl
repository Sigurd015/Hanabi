// --------------------------
// Phong Lighting Shader
// --------------------------

#type:vertex
#include "LightCommon.hlsl"

struct VertexInput
{
    float3 a_Position : a_Position;
    float3 a_Normal : a_Normal;
    float2 a_TexCoord : a_TexCoord;
};


struct VertexOutput
{
    float4 Position : SV_Position;
    float3 LocalPosition : LocalPos;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float3 CameraPosition : CamPos;
    DirectionalLight DirectionalLight : DirLight;
    PointLight PointLights[MAX_POINT_LIGHT] : PointLights;
    SpotLight SpotLights[MAX_SPOT_LIGHT] : SpotLights;
};

cbuffer CBModel : register(b0)
{
    float4x4 u_Transform;
};

cbuffer CBCamera : register(b1)
{
    float4x4 u_ViewProjection;
    float3 u_CameraPosition;
};

cbuffer CBScene : register(b2)
{
    DirectionalLight u_DirLight;
};

cbuffer CBPointLight : register(b3)
{
    PointLight u_PointLights[MAX_POINT_LIGHT];
    int u_PointLightsCount;
};

cbuffer CBSpotLight : register(b4)
{
    SpotLight u_SpotLights[MAX_SPOT_LIGHT];
    int u_SpotLightsCount;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.LocalPosition = Input.a_Position;
    Output.Position = mul(u_Transform, float4(Input.a_Position, 1.0f));
    Output.Position = mul(u_ViewProjection, Output.Position);
    
    Output.Normal = Input.a_Normal;
    Output.TexCoord = Input.a_TexCoord;

    Output.DirectionalLight = u_DirLight;
    Output.DirectionalLight.Direction = CalcLocal(u_Transform, u_DirLight.Direction);
    
    for (int i = 0; i < MAX_POINT_LIGHT; i++)
    {
        if (u_PointLights[i].Intensity > 0.0f)
        {
            Output.PointLights[i] = u_PointLights[i];
            Output.PointLights[i].Position = CalcLocal(u_Transform, u_PointLights[i].Position);
        }
    }
    
    for (int j = 0; j < MAX_SPOT_LIGHT; j++)
    {
        if (u_SpotLights[j].Intensity > 0.0f)
        {
            Output.SpotLights[j] = u_SpotLights[j];
            Output.SpotLights[j].Position = CalcLocal(u_Transform, u_SpotLights[j].Position);
            Output.SpotLights[j].Direction = CalcLocal(u_Transform, u_SpotLights[j].Direction);
        }
    }
    
    Output.CameraPosition = CalcLocal(u_Transform, u_CameraPosition);
    
    return Output;
}

#type:pixel
#include "LightCommon.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float3 LocalPosition : LocalPos;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float3 CameraPosition : CamPos;
    DirectionalLight DirectionalLight : DirLight;
    PointLight PointLights[MAX_POINT_LIGHT] : PointLights;
    SpotLight SpotLights[MAX_SPOT_LIGHT] : SpotLights;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

struct Material
{
    float3 DiffuseColor;
    float3 SpecularColor;
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
    
    Input.DirectionalLight.Direction = normalize(Input.DirectionalLight.Direction);
    Input.Normal = normalize(Input.Normal);
    float3 PixelToCamera = normalize(Input.CameraPosition - Input.LocalPosition);
    
    float3 dirLightColor = CalcDirectionalLight(Input.DirectionalLight, Input.Normal, material.DiffuseColor, material.SpecularColor, PixelToCamera);

    float3 pointLightColor = float3(0, 0, 0);
    for (int i = 0; i < MAX_POINT_LIGHT; i++)
    {
        if (Input.PointLights[i].Intensity > 0)
        {
            pointLightColor += CalcPointLight(Input.PointLights[i], Input.LocalPosition, material.DiffuseColor, material.SpecularColor, PixelToCamera);
        }
    };
    float3 spotLightColor = float3(0, 0, 0);
    for (int j = 0; j < MAX_SPOT_LIGHT; j++)
    {
        if (Input.SpotLights[j].Intensity > 0)
        {
            Input.SpotLights[j].Direction = normalize(Input.SpotLights[j].Direction);
            spotLightColor += CalcSpotLight(Input.SpotLights[j], Input.LocalPosition, material.DiffuseColor, material.SpecularColor, PixelToCamera);
        }
    };

    Output.Color = float4(saturate(dirLightColor + pointLightColor + spotLightColor), 1.0f);
    return Output;
}