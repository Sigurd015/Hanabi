#include "Buffers.hlsl"
#include "Common.hlsl"
#include "Lighting.hlsl"

Texture2D u_DirShadowMap : register(t8);
TextureCube u_PointShadowMap : register(t9);

float GetDirShadowBias()
{
	const float MINIMUM_SHADOW_BIAS = 0.002f;
	float bias = max(MINIMUM_SHADOW_BIAS * (1.0 - dot(m_Params.Normal, u_DirLight.Direction)), MINIMUM_SHADOW_BIAS);
	return bias;
}

float SearchRegionRadiusUV(float zWorld)
{
	const float light_zNear = 0.0; // 0.01 gives artifacts? maybe because of ortho proj?
	const float lightRadiusUV = 0.05;
	return lightRadiusUV * (zWorld - light_zNear) / zWorld;
}

static float ShadowFade = 1.0; //TODO(Karim): A better placement of this?

static const float2 PoissonDistribution[64] = {
	float2(-0.94201624, -0.39906216),
	float2(0.94558609, -0.76890725),
	float2(-0.094184101, -0.92938870),
	float2(0.34495938, 0.29387760),
	float2(-0.91588581, 0.45771432),
	float2(-0.81544232, -0.87912464),
	float2(-0.38277543, 0.27676845),
	float2(0.97484398, 0.75648379),
	float2(0.44323325, -0.97511554),
	float2(0.53742981, -0.47373420),
	float2(-0.26496911, -0.41893023),
	float2(0.79197514, 0.19090188),
	float2(-0.24188840, 0.99706507),
	float2(-0.81409955, 0.91437590),
	float2(0.19984126, 0.78641367),
	float2(0.14383161, -0.14100790),
	float2(-0.413923, -0.439757),
	float2(-0.979153, -0.201245),
	float2(-0.865579, -0.288695),
	float2(-0.243704, -0.186378),
	float2(-0.294920, -0.055748),
	float2(-0.604452, -0.544251),
	float2(-0.418056, -0.587679),
	float2(-0.549156, -0.415877),
	float2(-0.238080, -0.611761),
	float2(-0.267004, -0.459702),
	float2(-0.100006, -0.229116),
	float2(-0.101928, -0.380382),
	float2(-0.681467, -0.700773),
	float2(-0.763488, -0.543386),
	float2(-0.549030, -0.750749),
	float2(-0.809045, -0.408738),
	float2(-0.388134, -0.773448),
	float2(-0.429392, -0.894892),
	float2(-0.131597, 0.065058),
	float2(-0.275002, 0.102922),
	float2(-0.106117, -0.068327),
	float2(-0.294586, -0.891515),
	float2(-0.629418, 0.379387),
	float2(-0.407257, 0.339748),
	float2(0.071650, -0.384284),
	float2(0.022018, -0.263793),
	float2(0.003879, -0.136073),
	float2(-0.137533, -0.767844),
	float2(-0.050874, -0.906068),
	float2(0.114133, -0.070053),
	float2(0.163314, -0.217231),
	float2(-0.100262, -0.587992),
	float2(-0.004942, 0.125368),
	float2(0.035302, -0.619310),
	float2(0.195646, -0.459022),
	float2(0.303969, -0.346362),
	float2(-0.678118, 0.685099),
	float2(-0.628418, 0.507978),
	float2(-0.508473, 0.458753),
	float2(0.032134, -0.782030),
	float2(0.122595, 0.280353),
	float2(-0.043643, 0.312119),
	float2(0.132993, 0.085170),
	float2(-0.192106, 0.285848),
	float2(0.183621, -0.713242),
	float2(0.265220, -0.596716),
	float2(-0.009628, -0.483058),
	float2(-0.018516, 0.435703)
};

static const float2 poissonDisk[16] = {
	float2(-0.94201624, -0.39906216),
	float2(0.94558609, -0.76890725),
	float2(-0.094184101, -0.92938870),
	float2(0.34495938, 0.29387760),
	float2(-0.91588581, 0.45771432),
	float2(-0.81544232, -0.87912464),
	float2(-0.38277543, 0.27676845),
	float2(0.97484398, 0.75648379),
	float2(0.44323325, -0.97511554),
	float2(0.53742981, -0.47373420),
	float2(-0.26496911, -0.41893023),
	float2(0.79197514, 0.19090188),
	float2(-0.24188840, 0.99706507),
	float2(-0.81409955, 0.91437590),
	float2(0.19984126, 0.78641367),
	float2(0.14383161, -0.14100790)
};

float2 SamplePoisson(int index)
{
	return PoissonDistribution[index % 64];
}

// -------------------------- Directional Light Shadows --------------------------
float FindBlockerDistance_DirectionalLight(uint cascade, float3 shadowCoords, float uvLightSize)
{
	float bias = GetDirShadowBias();

	int numBlockerSearchSamples = 64;
	int blockers = 0;
	float avgBlockerDistance = 0;

	float searchWidth = SearchRegionRadiusUV(shadowCoords.z);
	for (int i = 0; i < numBlockerSearchSamples; i++)
	{
		float z = u_DirShadowMap.SampleLevel(u_SSLinearClamp, float3(shadowCoords.xy + SamplePoisson(i) * searchWidth, cascade), 0).r;
		if (z < (shadowCoords.z - bias))
		{
			blockers++;
			avgBlockerDistance += z;
		}
	}

	if (blockers > 0)
		return avgBlockerDistance / float(blockers);

	return -1;
}

float PCF_DirectionalLight(uint cascade, float3 shadowCoords, float uvRadius)
{
	float bias = GetDirShadowBias();
	int numPCFSamples = 64;

	float sum = 0;
	for (int i = 0; i < numPCFSamples; i++)
	{
		float2 offset = SamplePoisson(i) * uvRadius;
		float z = u_DirShadowMap.SampleLevel(u_SSLinearClamp, float3(shadowCoords.xy + offset, cascade), 0).r;
		sum += step(shadowCoords.z - bias, z);
	}
	return sum / numPCFSamples;
}

float PCSS_DirectionalLight(uint cascade, float3 shadowCoords, float uvLightSize)
{
	float blockerDistance = FindBlockerDistance_DirectionalLight(cascade, shadowCoords, uvLightSize);
	if (blockerDistance == -1) // No occlusion
		return 1.0f;

	float penumbraWidth = (shadowCoords.z - blockerDistance) / blockerDistance;

	float NEAR = 0.01; // Should this value be tweakable?
	float uvRadius = penumbraWidth * uvLightSize * NEAR / shadowCoords.z; // Do we need to divide by shadowCoords.z?
	uvRadius = min(uvRadius, 0.002f);
	return PCF_DirectionalLight(cascade, shadowCoords, uvRadius) * ShadowFade;
} 

float DirSoftShadow(float4 position)
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

	float bias = GetDirShadowBias();
    // PCF Sample with 8x8 kernel
    static const int SampleSize = 4;
    static float texelSize =0.00008f;

    float shadow = 0.0f;
    for (int i = -SampleSize; i <= SampleSize; i++)
    {
        for (int j = -SampleSize; j <= SampleSize; j++)
        {
            float pcfDepth = u_DirShadowMap.SampleLevel(u_SSLinearClamp, shadowPosition.xy + float2(i, j) * texelSize, 0).r;
            shadow += step(shadowPosition.z - bias , pcfDepth);
        }
    }

    // Number of iterations of inner + outer loop combined.
    return shadow / pow(SampleSize * 2 + 1, 2);

    //TODO: PCSS
    //return PCSS_DirectionalLight(0, shadowPosition.xyz, 0.00008f);
}

float DirHardShadow(float4 position)
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
    float bias = GetDirShadowBias();
    float depthFromLight = u_DirShadowMap.SampleLevel(u_SSLinearClamp, shadowPosition.xy, 0).r;
    return step(shadowPosition.z - bias, depthFromLight);
}
// -------------------------- Directional Light Shadows --------------------------

// -------------------------- Point Light Shadows --------------------------
float PointHardShadow(float3 position)
{
	const float bias = 0.001f;
	float3 pixelToLight = position - u_PointLightPosition;
	float depthFromLight = u_PointShadowMap.SampleLevel(u_SSLinearClamp, pixelToLight, 0).r;
	depthFromLight *= u_PointFarPlane;
	return step(length(pixelToLight) - bias, depthFromLight);
}
// -------------------------- Point Light Shadows --------------------------