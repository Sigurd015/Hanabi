//--------------------------
// Renderer2D Line Shader
// --------------------------

#pragma stage : vertex
struct VertexInput
{
    float3 a_Position : a_Position;
    float4 a_Color : a_Color;
};

struct VertexOutput
{
    float4 Pos : SV_Position;
    float4 Color : Cor;
};

cbuffer CBCamera : register(b0)
{
    float4x4 u_ViewProjection;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.Color = Input.a_Color;
    Output.Pos = mul(u_ViewProjection, float4(Input.a_Position, 1.0f));
    return Output;
}

#pragma stage : pixel
struct PixelInput
{
    float4 Pos : SV_Position;
    float4 Color : Cor;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    Output.Color = Input.Color;
    return Output;
}