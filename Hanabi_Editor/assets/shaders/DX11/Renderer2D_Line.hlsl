//--------------------------
// Renderer2D Line Shader
// --------------------------

#type:vertex
struct VertexInput
{
	float3 a_Position : a_Position;
	float4 a_Color : a_Color;
	int a_EntityID : a_EntityID;
};

struct VertexOutput
{
	float4 Pos : SV_Position;
	float4 Color : Cor;
	int EntityID : EID;
};

cbuffer Camera : register(b0)
{
	float4x4 u_ViewProjection;
};

VertexOutput main(VertexInput Input)
{
	VertexOutput Output;
	Output.Color = Input.a_Color;
	Output.EntityID = Input.a_EntityID;
	Output.Pos = mul(float4(Input.a_Position, 1.0f), u_ViewProjection);
	return Output;
}

#type:pixel
struct PixelInput
{
	float4 Color : Cor;
	int EntityID : EID;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
	int EntityID : SV_Target1;
};

PixelOutput main(PixelInput Input)
{
	PixelOutput Output;
	Output.Color = Input.Color;
	Output.EntityID = Input.EntityID;
	return Output;
}