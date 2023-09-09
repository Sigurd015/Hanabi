#ifndef SHADOW_HEADER
#define SHADOW_HEADER

#include "Buffers.hlsl"

Texture2D u_ShadowDepth : register(t3);

SamplerState u_SSLinearClamp : register(s1);

// PCF Soft Shadows
float CalculateSoftShadow(float4 position)
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

    float currentDepth = shadowPosition.z;

    float shadow = 0.0f;
    float bias = 0.005f; // max(0.05f * (1.0f -  dot(input.normal, lightDirection.xyz)), 0.005f);
    float texelSize =0.00008f;

    // PCF Sample with 6x6 kernel
    static const int SampleSize = 3;

    for (int i = -SampleSize; i <= SampleSize; i++)
    {
        for (int j = -SampleSize; j <= SampleSize; j++)
        {
            float pcfDepth = u_ShadowDepth.Sample(u_SSLinearClamp, shadowPosition.xy + float2(i, j) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 0.0f : 1.0f;
        }
    }

    // Number of iterations of inner + outer loop combined.
    return shadow / pow(SampleSize * SampleSize, 2);
}

// Hard Shadows
float CalculateHardShadow(float4 position)
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

    float currentDepth = shadowPosition.z;

    float shadow = 0.0f;
    float bias = 0.005f; 
    float depthFromLight = u_ShadowDepth.Sample(u_SSLinearClamp, shadowPosition.xy).r;
    if(currentDepth > depthFromLight + bias)
    {
        shadow = 1.0f;
    }
    return 1.0f - shadow;
}
#endif