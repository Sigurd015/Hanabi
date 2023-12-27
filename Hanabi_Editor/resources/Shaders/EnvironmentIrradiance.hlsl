// --------------------------
// Environment Irradiance Compute Shader
// --------------------------

#pragma stage : compute
#include "Include/EnvironmentMapping.hlsl"

// Computes diffuse irradiance cubemap convolution for image-based lighting.
// Uses quasi Monte Carlo sampling with Hammersley sequence.

Texture2D u_RadianceMap : register(t0);

RWTexture2DArray<float4> o_IrradianceMap : register(u0);

cbuffer CBSamplesParams : register(b0)
{
    uint u_Samples;
};

[numthreads(32, 32, 1)]
void main(uint3 ThreadID : SV_DispatchThreadID)
{
    float outputWidth, outputHeight, outputDepth;
    o_IrradianceMap.GetDimensions(outputWidth, outputHeight, outputDepth);
    float3 N = GetCubeMapTexCoord(ThreadID, float2(outputWidth, outputHeight));

    float3 S, T;
    ComputeBasisVectors(N, S, T);

    uint samples = 64 * u_Samples;

    // Monte Carlo integration of hemispherical irradiance.
	// As a small optimization this also includes Lambertian BRDF assuming perfectly white surface (albedo of 1.0)
	// so we don't need to normalize in PBR fragment shader (so technically it encodes exitant radiance rather than irradiance).
    float3 irradiance = 0;
    for (uint i = 0; i < samples; i++)
    {
        float2 u = SampleHammersley(i, samples);
        float3 Li = TangentToWorld(SampleHemisphere(u.x, u.y), N, S, T);
        float cosTheta = max(0.0, dot(Li, N));

		// PIs here cancel out because of division by pdf.
        irradiance += 2.0 * u_RadianceMap.SampleLevel(u_SSLinearWrap, Li, 0).rgb * cosTheta;
    }
    irradiance /= samples;

	// Write out color to output cubemap.
    o_IrradianceMap[ThreadID] = float4(irradiance, 1.0);
}