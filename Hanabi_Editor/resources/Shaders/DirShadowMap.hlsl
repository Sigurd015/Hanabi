// --------------------------
// DirShadow Map Shader
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
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    float3 worldPos = mul(u_Transform, float4(Input.a_Position, 1.0f));
    Output.Position = mul(u_DirLightViewProjection, float4(worldPos, 1.0));
    return Output;
}

#type:pixel

void main()
{
    // TODO: Check for alpha in texture
}