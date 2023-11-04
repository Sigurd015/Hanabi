#ifndef COMMON_HEADER
#define COMMON_HEADER

static const float PI = 3.141592f;
static const float TwoPI = 2 * PI;
static const float Epsilon = 0.00001f;

// Common sampler states
SamplerState u_SSLinearWrap : register(s0);
SamplerState u_SSLinearClamp : register(s1);
SamplerState u_SSAnisotropicWrap : register(s2);

// Gamma correction
float3 GammaCorrect(float3 color, float gamma)
{
    return pow(color, 1.0f / gamma);
}

float4 SRGBToLinear(float4 color)
{
    bool4 cutoff = bool4(color.r < 0.04045f, color.g < 0.04045f, color.b < 0.04045f, color.a < 0.04045f);
    float4 higher = pow((color + 0.055f) / 1.055f, 2.4f);
    float4 lower = color / 12.92f;

    return lerp(higher, lower, cutoff);
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