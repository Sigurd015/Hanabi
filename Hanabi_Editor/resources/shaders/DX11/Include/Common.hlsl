#ifndef COMMON_HEADER
#define COMMON_HEADER

const float PI = 3.141592;

// Gamma correction
float3 LinearToSRGB(float3 color)
{
    return pow(color, 1.0 / 2.2);
}

float3 SRGBToLinear(float3 color)
{
    return pow(color, 2.2);
}

#endif