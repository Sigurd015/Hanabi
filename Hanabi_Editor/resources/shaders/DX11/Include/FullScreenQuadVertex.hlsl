// --------------------------
// Full Screen Quad Vertex Shader
// --------------------------

struct VertexInput
{
    float3 a_Position : a_Position;
    float2 a_TexCoord : a_TexCoord;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 TexCoord : Tex;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.Position = float4(input.a_Position, 1.0f);
    output.TexCoord = input.a_TexCoord;
    return output;
}