// --------------------------
// Shadow Mapping Shader
// --------------------------

#type:vertex
#include "FullScreenQuadVertex.hlsl"

#type:pixel
#include "Buffers.hlsl"
#include "ShadowMappingUtils.hlsl"

Texture2D u_PositionBuffer : register(t1);
Texture2D u_PrevShadowFactorBuffer : register(t2);

// Calculate the shadow factor for all lights
// R channel: point light shadow factor
float4 main(float4 Position : SV_Position) : SV_Target
{
    float3 WorldPosition = u_PositionBuffer.Load(uint3(Position.xy, 0)).xyz;
    float3 PrevShadowFactor = u_PrevShadowFactorBuffer.Load(uint3(Position.xy, 0)).xyz;

    float pointShadowFactor = 1.0f;
    switch (u_PointShadowType)
    {
        case 1:
            pointShadowFactor = PointHardShadow(WorldPosition);
            break;
        case 2:
            pointShadowFactor = PointHardShadow(WorldPosition);
            break;
    }
    return float4(PrevShadowFactor * pointShadowFactor, 0.0f, 0.0f, 1.0f);
}