#ifndef SHADOW_HEADER
#define SHADOW_HEADER
#include "Buffers.hlsl"
#include "Common.hlsl"

Texture2D u_ShadowMap : register(t8);

float GetDirShadowBias(float3 worldNormal)
{
	const float MINIMUM_SHADOW_BIAS = 0.002f;
	float bias = max(MINIMUM_SHADOW_BIAS * (1.0 - dot(worldNormal, u_DirLight.Direction)), MINIMUM_SHADOW_BIAS);
	return bias;
}

// PCF Soft Shadows
float CalculateSoftShadow(float4 position, float3 worldNormal)
{
    float4 shadowPosition = position;
    shadowPosition.xyz /= shadowPosition.w;
    
    // Check if the point is outside the shadow map
    if (shadowPosition.z > 1.0f)
    {
        return 0.0f;
    }

    // Transform from ndc [-1, 1] to uv [0, 1]
    shadowPosition.x = 0.5f * shadowPosition.x + 0.5f;
    shadowPosition.y = -0.5f * shadowPosition.y + 0.5f;

    float bias = GetDirShadowBias(worldNormal);
    // PCF Sample with 8x8 kernel
    static const int SampleSize = 4;
    static float texelSize =0.00008f;

    float shadow = 0.0f;
    for (int i = -SampleSize; i <= SampleSize; i++)
    {
        for (int j = -SampleSize; j <= SampleSize; j++)
        {
            float pcfDepth = u_ShadowMap.SampleLevel(u_SSLinearClamp, shadowPosition.xy + float2(i, j) * texelSize, 0).r;
            shadow += step(shadowPosition.z - bias , pcfDepth);
        }
    }

    // Number of iterations of inner + outer loop combined.
    return shadow / pow(SampleSize * 2 + 1, 2);
}

// Hard Shadows
float CalculateHardShadow(float4 position, float3 worldNormal)
{
    float4 shadowPosition = position;
    shadowPosition.xyz /= shadowPosition.w;

    // Check if the point is outside the shadow map
    if (shadowPosition.z > 1.0f)
    {
        return 0.0f;
    }

    shadowPosition.x = 0.5f * shadowPosition.x + 0.5f;
    shadowPosition.y = -0.5f * shadowPosition.y + 0.5f;

    float shadow = 1.0f;
    float bias = GetDirShadowBias(worldNormal);
    float depthFromLight = u_ShadowMap.SampleLevel(u_SSLinearClamp, shadowPosition.xy, 0).r;
    return step(shadowPosition.z, depthFromLight + bias);
}
#endif