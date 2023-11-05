#ifndef COMMON_HEADER
#define COMMON_HEADER

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

float3 RotateVectorAboutY(float angle, float3 vec)
{
	angle = radians(angle);
	float3x3 rotationMatrix = { float3( cos(angle), 0.0, sin(angle)),
							    float3(       0.0,  1.0,        0.0),
							    float3(-sin(angle), 0.0, cos(angle)) };
	return mul(rotationMatrix, vec);
}

#endif