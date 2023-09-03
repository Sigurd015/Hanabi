#ifndef SHADOW_HEADER
#define SHADOW_HEADER

#include "Buffers.hlsl"
#include "Textures.hlsl"

float CalculateShadow(float4 position)
{
    float4 shadowPosition = position;
    shadowPosition.xyz /= shadowPosition.w;

    shadowPosition.x = 0.5f * shadowPosition.x + 0.5f;
    shadowPosition.y = -0.5f * shadowPosition.y + 0.5f;

    if (shadowPosition.z > 1.0f)
    {
        return 0.0f;
    }

    float currentDepthValue = shadowPosition.z;

    float shadow = 0.0f;
    float bias = 0.005f; // max(0.05f * (1.0f -  dot(input.normal, lightDirection.xyz)), 0.005f);
    float texelSize =0.00008f;

    static const int KERNEL_WIDTH = 3;

    for (int i = -KERNEL_WIDTH; i <= KERNEL_WIDTH; i++)
    {
        for (int j = -KERNEL_WIDTH; j <= KERNEL_WIDTH; j++)
        {
            float pcfDepth = u_ShadowDepth.Sample(u_SSLinearWrap, shadowPosition.xy + float2(i, j) * texelSize).r;
            shadow += currentDepthValue - bias > pcfDepth ? 0.0f : 1.0f;
        }
    }

    // Number of iterations of inner + outer loop combined.
    return shadow / pow(KERNEL_WIDTH * KERNEL_WIDTH, 2);
}
#endif