#ifndef BUFFER_HEADER
#define BUFFER_HEADER
#define MAX_POINT_LIGHT 32
#define MAX_SPOT_LIGHT 32

struct DirectionalLight
{
    float3 Radiance;
    float Intensity;
    float3 Direction;
};

struct PointLight
{
    float3 Position;
    float Intensity;
    float3 Radiance;
    float Radius;
    float Falloff;
};

struct SpotLight
{
    float3 Position;
    float Intensity;
    float3 Radiance;
    float AngleAttenuation;
    float3 Direction;
    float Range;
    float Angle;
    float Falloff;
};

struct Material
{
    float3 Albedo;
    float Emission;
    float Metalness;
	float Roughness;
    bool UseNormalMap;
};

cbuffer CBModel : register(b0)
{
    float4x4 u_Transform;
    Material u_Material;
};

cbuffer CBCamera : register(b1)
{
    float4x4 u_ViewProjection;
    float3 u_CameraPosition;
};

cbuffer CBScene : register(b2)
{
    float u_SkyLightIntensity;
    DirectionalLight u_DirLight;
};

cbuffer CBPointLight : register(b3)
{
    uint u_PointLightsCount;
    PointLight u_PointLights[MAX_POINT_LIGHT];
};

cbuffer CBSpotLight : register(b4)
{
    uint u_SpotLightsCount;
    SpotLight u_SpotLights[MAX_SPOT_LIGHT];
};

cbuffer CBShadow : register(b5)
{
    float4x4 u_LightViewProjection;
    uint u_ShadowType;
    uint u_LightType;
};
#endif