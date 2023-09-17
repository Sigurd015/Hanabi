// --------------------------
// Shadow Mapping Shader
// --------------------------

#type:vertex
#include "FullScreenQuadVertex.hlsl"

#type:pixel
#include "Buffers.hlsl"

Texture2D u_ShadowDepth : register(t5);
SamplerState u_SSLinearClamp : register(s0);

#include "ShadowMappingUtils.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float2 TexCoord : Tex;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D u_NormalBuffer : register(t6);
Texture2D u_PositionBuffer : register(t7);
Texture2D u_LightResult : register(t8);

SamplerState u_SSLinearWrap : register(s1);

PixelOutput main(PixelInput Input)
{
    PixelOutput output;
    float2 texCoord = float2(Input.TexCoord.x, 1.0 - Input.TexCoord.y);
    float3 lightResult = u_LightResult.Sample(u_SSLinearWrap, texCoord).xyz;
    float3 normal = u_NormalBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    float4 position = u_PositionBuffer.Sample(u_SSLinearWrap, texCoord);
    float4 lightTransformedPosition = mul(position, u_LightViewProjection);

    float shadowResult = 1.0f;
    switch (u_LightType)
    {
        case 1: // Directional
            switch (u_ShadowType)
            {
                case 1:
                    shadowResult = CalculateHardShadow(lightTransformedPosition, normal);
                    break;
                case 2:
                    shadowResult = CalculateSoftShadow(lightTransformedPosition, normal);
                    break;
            }
            break;
    }

    output.Color = float4(lightResult * shadowResult, 1.0f);
    return output;
}

