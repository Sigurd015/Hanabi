#ifndef COMMON_HEADER
#define COMMON_HEADER

const float PI = 3.141592;
const float Epsilon = 0.00001;

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