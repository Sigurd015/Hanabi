// --------------------------
// PointShadow Map Shader
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
    float4 Position : SV_Position;
    float2 TexCoord : TexCoord;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.Position = mul(u_Transform, float4(Input.a_Position, 1.0f));
    Output.TexCoord = Input.a_TexCoord;
    return Output;
}

#type:geometry
#include "Buffers.hlsl"

struct GSInput
{
    float4 Vertex : SV_Position;
    float2 TexCoord : TexCoord;
};

struct GSOutput
{
    float4 Vertex : SV_Position;
    float2 TexCoord : TexCoord;
    float3 WorldPosition : WorldPosition;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)] //6 * 3 = 18
void main(
	triangle GSInput Input[3], //Takes the output from vertex shader * 3
	inout TriangleStream<GSOutput> OutputStream //Sends to pixel shader
)
{
    for (uint CubeFaceIndex = 0; CubeFaceIndex < 6; CubeFaceIndex++)
    {
        GSOutput output;
        output.RTIndex = CubeFaceIndex; //Keep track of cube face
        
        //Triangle = 3 vertices
        for (int VertexIndex = 0; VertexIndex < 3; VertexIndex++)
        {
            output.WorldPosition = Input[VertexIndex].Vertex.xyz;
            output.TexCoord = Input[VertexIndex].TexCoord;
            output.Vertex = mul(u_PointLightViewProj[CubeFaceIndex], float4(Input[VertexIndex].Vertex.xyz, 1.0f));
            OutputStream.Append(output);
        }
        OutputStream.RestartStrip();
    }
}

#type:pixel
#include "Common.hlsl"
#include "Buffers.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float2 TexCoord : TexCoord;
    float3 WorldPosition : WorldPosition;
};

Texture2D u_AlbedoTex : register(t0);

float main(PixelInput Input) : SV_Depth
{
    float4 albedo = u_AlbedoTex.Sample(u_SSAnisotropicWrap, Input.TexCoord);
    // Discard transparent pixels
    clip(albedo.a - 0.01f);

    // get distance between pixel and light source
    float lightDistance = length(Input.WorldPosition - u_PointLightPosition);

    // map to [0;1] range by dividing by far plane, then write this as modified depth
    return lightDistance / u_PointFarPlane;
}