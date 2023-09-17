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
    switch(u_LightType)
    {
        case 0: // Directional
        {
           	float3 worldPos = mul(u_Transform, float4(Input.a_Position, 1.0f));
            Output.Position = mul(u_LightViewProjection, float4(worldPos, 1.0));
            break;
        }
    }

    return Output;
}

#type:pixel

void main()
{
    // TODO: Check for alpha in texture
}