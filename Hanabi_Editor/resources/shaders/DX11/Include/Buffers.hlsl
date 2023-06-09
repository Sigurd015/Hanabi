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

cbuffer CBModel : register(b0)
{
    float4x4 u_Transform;
    bool u_UseNormalMap;
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
    uint u_PointLightsCount;
    PointLight u_PointLights[MAX_POINT_LIGHT];
};

cbuffer CBSpotLight : register(b4)
{
    uint u_SpotLightsCount;
    SpotLight u_SpotLights[MAX_SPOT_LIGHT];
};
#endif