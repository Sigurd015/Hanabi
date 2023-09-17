//--------------------------
// Renderer2D Quad Shader
// --------------------------

#type:vertex
struct VertexInput
{
    float3 a_Position : a_Position;
    float4 a_Color : a_Color;
    float2 a_TexCoord : a_TexCoord;
    int a_TexIndex : a_TexIndex;
    float a_TilingFactor : a_TilingFactor;
};

struct VertexOutput
{
    float4 Pos : SV_Position;
    float4 Color : Cor;
    float2 TexCoord : Ted;
    int TexIndex : Tei;
    float TilingFactor : Tlf;
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
    Output.TexIndex = Input.a_TexIndex;
    Output.TilingFactor = Input.a_TilingFactor;
    Output.Pos = mul(u_ViewProjection, float4(Input.a_Position, 1.0f));
    return Output;
}

#type:pixel
#define MAX_TEXTURE 32

struct PixelInput
{
    float4 Pos : SV_Position;
    float4 Color : Cor;
    float2 TexCoord : Ted;
    int TexIndex : Tei;
    float TilingFactor : Tlf;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D u_Textures[MAX_TEXTURE] : register(t0);
SamplerState u_SSLinearWrap : register(s0);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    float4 texColor;
    switch (Input.TexIndex)
    {
        case 0:
            texColor = u_Textures[0].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 1:
            texColor = u_Textures[1].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 2:
            texColor = u_Textures[2].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 3:
            texColor = u_Textures[3].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 4:
            texColor = u_Textures[4].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 5:
            texColor = u_Textures[5].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 6:
            texColor = u_Textures[6].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 7:
            texColor = u_Textures[7].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 8:
            texColor = u_Textures[8].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 9:
            texColor = u_Textures[9].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 10:
            texColor = u_Textures[10].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 11:
            texColor = u_Textures[11].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 12:
            texColor = u_Textures[12].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 13:
            texColor = u_Textures[13].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 14:
            texColor = u_Textures[14].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
        case 15:
            texColor = u_Textures[15].Sample(u_SSLinearWrap, Input.TexCoord * Input.TilingFactor) * Input.Color;
            break;
    }

    if (texColor.a == 0.0)
        discard;
	
    Output.Color = texColor;
    return Output;
}