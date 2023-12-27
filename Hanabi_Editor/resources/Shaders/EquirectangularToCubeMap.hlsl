// --------------------------
// Equirectangular To CubeMap Compute Shader
// --------------------------

#pragma stage : compute
#include "Include/EnvironmentMapping.hlsl"

Texture2D u_EquirectangularMap : register(t0);

RWTexture2DArray<float4> o_OutputTex : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 ThreadID : SV_DispatchThreadID)
{
    float width, height, level;
    o_OutputTex.GetDimensions(width, height, level);
    float3 cubeTC = GetCubeMapTexCoord(ThreadID, float2(width, height));
	
    // Calculate sampling coords for equirectangular texture
	// https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
    float phi = atan2(cubeTC.z, cubeTC.x);
    float theta = acos(cubeTC.y);
    float2 uv = float2(phi / (TwoPI) + 0.5, theta / PI);
	
    // Sample equirectangular texture.
    float4 color = u_EquirectangularMap.SampleLevel(u_SSLinearWrap, uv, 0);

	// Write out color to output cubemap.
    o_OutputTex[ThreadID] = color;
}