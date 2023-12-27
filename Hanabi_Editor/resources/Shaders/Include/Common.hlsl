#pragma once
static const float PI = 3.141592f;
static const float TwoPI = 2 * PI;
static const float Epsilon = 0.00001f;

// Common sampler states
SamplerState u_SSPointClamp : register(s0);
SamplerState u_SSLinearWrap : register(s1);
SamplerState u_SSLinearClamp : register(s2);
SamplerState u_SSAnisotropicWrap : register(s3);

// Gamma correction
float3 GammaCorrect(float3 color, float gamma)
{
    return pow(color, 1.0f / gamma);
}

float3 SRGBToLinear(float3 color, float gamma)
{
    return pow(color, gamma);
}

// Based on http://www.oscars.org/science-technology/sci-tech-projects/aces
float3 ACESTonemap(float3 color)
{
    float3x3 m1 = float3x3(
		0.59719, 0.07600, 0.02840,
		0.35458, 0.90834, 0.13383,
		0.04823, 0.01566, 0.83777
	);
    float3x3 m2 = float3x3(
		1.60475, -0.10208, -0.00327,
		-0.53108, 1.10813, -0.07276,
		-0.07367, -0.00605, 1.07602
	);
    float3 v = mul(transpose(m1), color);
    float3 a = v * (v + 0.0245786) - 0.000090537;
    float3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return clamp(mul(transpose(m2), (a / b)), 0.0, 1.0);
}

float3 RotateVectorAboutY(float angle, float3 vec)
{
    angle = radians(angle);
    float3x3 rotationMatrix =
    {
        float3(cos(angle), 0.0, sin(angle)),
		float3(0.0, 1.0, 0.0),
		float3(-sin(angle), 0.0, cos(angle))
    };
    return mul(transpose(rotationMatrix), vec);
}