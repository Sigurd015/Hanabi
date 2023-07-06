// --------------------------
// SpotLight Shadow Map Shader
// --------------------------

#type:vertex
#include "Buffers.hlsl"

struct VertexInput
{
    float3 a_Position : a_Position;
    float3 a_Normal : a_Normal;
    float3 a_Tangent : a_Tangent;
    float3 a_Bitangent : a_Bitangent;
    float2 a_TexCoord : a_TexCoord;
};

struct VertexOutput
{

};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;

    return Output;
}

#type:pixel

void main()
{}
