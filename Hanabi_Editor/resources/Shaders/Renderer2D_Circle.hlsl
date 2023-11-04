// --------------------------
// Renderer2D Circle Shader
// --------------------------

#type:vertex
struct VertexInput
{
    float3 a_WorldPosition : a_WorldPosition;
    float3 a_LocalPosition : a_LocalPosition;
    float4 a_Color : a_Color;
    float a_Thickness : a_Thickness;
    float a_Fade : a_Fade;
};

struct VertexOutput
{
    float4 Pos : SV_Position;
    float3 LocalPosition : LoP;
    float4 Color : Cor;
    float Thickness : Ths;
    float Fade : Fde;
};

cbuffer CBCamera : register(b0)
{
    float4x4 u_ViewProjection;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.LocalPosition = Input.a_LocalPosition;
    Output.Color = Input.a_Color;
    Output.Thickness = Input.a_Thickness;
    Output.Fade = Input.a_Fade;
    Output.Pos = mul(u_ViewProjection, float4(Input.a_WorldPosition, 1.0f));
    return Output;
}

#type:pixel
struct PixelInput
{
    float4 Pos : SV_Position;
    float3 LocalPosition : LoP;
    float4 Color : Cor;
    float Thickness : Ths;
    float Fade : Fde;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
	
    // Calculate distance and fill circle with white
    float distance = 1.0f - length(Input.LocalPosition);
    float circle = smoothstep(0.0f, Input.Fade, distance);
    circle *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, distance);
	
    if (circle == 0.0f)
        discard;
		
    Output.Color = Input.Color;
    Output.Color.a *= circle;
    return Output;
}