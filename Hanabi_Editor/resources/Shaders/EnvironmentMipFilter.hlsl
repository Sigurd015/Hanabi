// --------------------------
// Environment Mip Filter Compute Shader
// --------------------------

#pragma stage : compute
#include "Include/EnvironmentMapping.hlsl"
#include "Include/PBRUtils.hlsl"

static const uint NumSamples = 1024;
static const float InvNumSamples = 1.0 / float(NumSamples);

float2 SampleHammersley(uint i)
{
    return float2(i * InvNumSamples, RadicalInverse_VdC(i));
}

TextureCube u_InputTex : register(t0);
RWTexture2DArray<float4> o_OutputTex : register(u0);

cbuffer CBFilterParam : register(b0)
{
    float u_Roughness;
};

[numthreads(32, 32, 1)]
void main(uint3 ThreadID : SV_DispatchThreadID)
{
    // Make sure we won't write past output when computing higher mipmap levels.
    float outputWidth, outputHeight, outputLevel;
    o_OutputTex.GetDimensions(outputWidth, outputHeight, outputLevel);
    if (ThreadID.x >= outputWidth || ThreadID.y >= outputHeight)
    {
        return;
    }

    // Solid angle associated with a single cubemap texel at zero mipmap level.
	// This will come in handy for importance sampling below.
    float inputWidth, inputHeight, inputLevel;
    u_InputTex.GetDimensions(0, inputWidth, inputHeight, inputLevel);

    float wt = 4.0 * PI / (6 * inputWidth * inputHeight);

	// Approximation: Assume zero viewing angle (isotropic reflections).
    float3 N = GetCubeMapTexCoord(ThreadID, float2(outputWidth, outputHeight));
    float3 Lo = N;

    float3 S, T;
    ComputeBasisVectors(N, S, T);

    float3 color = 0;
    float weight = 0;

    // Convolve environment map using GGX NDF importance sampling.
	// Weight by cosine term since Epic claims it generally improves quality.
    for (uint i = 0; i < NumSamples; i++)
    {
        float2 u = SampleHammersley(i);
        float3 Lh = TangentToWorld(SampleGGX(u.x, u.y, u_Roughness), N, S, T);

		// Compute incident direction (Li) by reflecting viewing direction (Lo) around half-vector (Lh).
        float3 Li = 2.0 * dot(Lo, Lh) * Lh - Lo;

        float cosLi = dot(N, Li);
        if (cosLi > 0.0)
        {
			// Use Mipmap Filtered Importance Sampling to improve convergence.
			// See: https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html, section 20.4

            float cosLh = max(dot(N, Lh), 0.0);

			// GGX normal distribution function (D term) probability density function.
			// Scaling by 1/4 is due to change of density in terms of Lh to Li (and since N=V, rest of the scaling factor cancels out).
            float pdf = NdfGGX(cosLh, u_Roughness) * 0.25;

			// Solid angle associated with this sample.
            float ws = 1.0 / (NumSamples * pdf);

			// Mip level to sample from.
            float mipLevel = max(0.5 * log2(ws / wt) + 1.0, 0.0);

            color += u_InputTex.SampleLevel(u_SSLinearWrap, Li, mipLevel).rgb * cosLi;
            weight += cosLi;
        }
    }
    color /= weight;

    o_OutputTex[ThreadID] = float4(color, 1.0);
}