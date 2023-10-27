#ifndef COMMON_HEADER
#define COMMON_HEADER

static const float PI = 3.141592f;
static const float TwoPI = 2 * PI;
static const float Epsilon = 0.00001f;

// Gamma correction
float3 LinearToSRGB(float3 color, float gamma)
{
    float temp = 1.0f / gamma;
    return pow(color, float3(temp, temp, temp));
}

float3 SRGBToLinear(float3 color)
{
    return pow(color, float3(2.2f, 2.2f, 2.2f));
}

#endif