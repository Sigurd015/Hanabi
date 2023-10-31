// --------------------------
// Composite Shader
// --------------------------

#type:vertex
#include "FullScreenQuadVertex.hlsl"

#type:pixel
#include "Common.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float2 TexCoord : Tex;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D u_Color : register(t0);
SamplerState u_SSLinearWrap : register(s0);

PixelOutput main(PixelInput Input)
{
    PixelOutput output;
    float2 texCoord = float2(Input.TexCoord.x, 1.0 - Input.TexCoord.y);
    float4 color = u_Color.Sample(u_SSLinearWrap, texCoord);
    output.Color.xyz = LinearToSRGB(color.xyz);
    //output.Color = color;
    return output;
}