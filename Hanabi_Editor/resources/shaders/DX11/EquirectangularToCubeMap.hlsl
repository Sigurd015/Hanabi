// --------------------------
// Equirectangular To CubeMap Shader
// --------------------------

#type:vertex
#include "Buffers.hlsl"

struct VertexInput
{
    float3 a_Position : a_Position;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 LocalPosition : LoP;
};

VertexOutput main(VertexInput Input)
{
	VertexOutput Output;
	Output.Position = mul(u_ViewProjection, float4(Input.a_Position, 1.0));
	Output.LocalPosition = Input.a_Position;
	return Output;
}

#type:pixel

struct PixelInput
{
    float4 Position : SV_Position;
    float3 LocalPosition : LoP;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

static const float2 invAtan = float2(0.1591, 0.3183);
float2 SampleSphericalMap(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

Texture2D u_EquirectangularMap : register(t0);

SamplerState u_SSLinearWrap : register(s0);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
	float2 texCoord = SampleSphericalMap(normalize(Input.LocalPosition));
	Output.Colour = float4(u_EquirectangularMap.Sample(u_SSLinearWrap, texCoord).rgb, 1.0f);
    return Output;
}
