#ifndef COMMON_HEADER
#define COMMON_HEADER

static const float PI = 3.141592f;
static const float TwoPI = 2 * PI;
static const float Epsilon = 0.00001f;

// Gamma correction
float3 LinearToSRGB(float3 color)
{
    return (color <= 0.0031308f) ? color * 12.92f : pow(color, 1.f / 2.2f) * 1.055f - 0.055f;
    //float temp = 1.0f / 2.2f;
    //return pow(color, float3(temp, temp, temp));
}

float3 SRGBToLinear(float3 color)
{
    return (color <= 0.04045f) ? color / 12.92f : pow((color + 0.055f) / 1.055f, 2.2f);
    //return pow(color, float3(2.2f, 2.2f, 2.2f));
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