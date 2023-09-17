//--------------------------
// Renderer2D MSDF text shader
// --------------------------

#type:vertex
struct VertexInput
{
	float3 a_Position : a_Position;
	float4 a_Color : a_Color;
	float2 a_TexCoord : a_TexCoord;
};

struct VertexOutput
{
	float4 Pos : SV_Position;
	float4 Color : Cor;
	float2 TexCoord : Ted;
};

cbuffer CBCamera : register(b0)
{
	float4x4 u_ViewProjection;
};

VertexOutput main(VertexInput Input)
{
	VertexOutput Output;
	Output.Color = Input.a_Color;
	Output.TexCoord = Input.a_TexCoord;
	Output.Pos = mul(u_ViewProjection,float4(Input.a_Position,1.0f));
	return Output;
}

#type:pixel
struct PixelInput
{
    float4 Pos : SV_Position;
    float4 Color : Cor;
    float2 TexCoord : Ted;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D u_FontAtlas : register(t0);
SamplerState u_SSLinearWrap : register(s0);

float screenPxRange(PixelInput Input)
{
    const float pxRange = 2.0; // set to distance field's pixel range
    uint width, height;
    u_FontAtlas.GetDimensions(width, height);
    float2 unitRange = float2(pxRange, pxRange) / float2(width, height);
    float2 screenTexSize = float2(1.0, 1.0) / ddx(Input.TexCoord);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    float4 temp = u_FontAtlas.Sample(u_SSLinearWrap, Input.TexCoord);
    float4 texColor = temp * Input.Color;
    float3 msd = temp.rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange(Input) * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    if (opacity == 0.0)
        discard;

    float4 bgColor = float4(0.0, 0.0, 0.0, 0.0);
    Output.Color = lerp(bgColor, Input.Color, opacity);
    if (Output.Color.a == 0.0)
        discard;

    return Output;
}