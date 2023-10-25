// --------------------------
// Equirectangular To CubeMap Shader
// --------------------------

#type:compute
#include "EnvironmentMapping.hlsl"

Texture2D u_EquirectangularMap : register(t0);
SamplerState u_SSLinearWrap : register(s0);

RWTexture2DArray<float4> o_Tex : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 ThreadID : SV_DispatchThreadID)
{
    float outputWidth, outputHeight, outputDepth;
    o_Tex.GetDimensions(outputWidth, outputHeight, outputDepth);
    float3 cubeTC = GetCubeMapTexCoord(ThreadID, float2(outputWidth, outputHeight));
	
    // Calculate sampling coords for equirectangular texture
	// https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
	float phi = atan2(cubeTC.z, cubeTC.x);
	float theta = acos(cubeTC.y);
    float2 uv = float2(phi / (2.0 * PI) + 0.5, theta / PI);
	
    // Sample equirectangular texture.
    float4 color = u_EquirectangularMap.SampleLevel(u_SSLinearWrap, uv, 0);

	// Write out color to output cubemap.
    o_Tex[ThreadID] = color;
}