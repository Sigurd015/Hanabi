// --------------------------
// 3D Static Mesh Default Shader
// --------------------------

#type:vertex
struct VertexInput
{
    float3 a_Position : a_Position;
    float3 a_Normal : a_Normal;
    float2 a_Texcoord : a_Texcoord;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal : Nor;
    float2 Texcoord : Tex;
};

cbuffer Camera : register(b0)
{
    float4x4 u_Model;
    float4x4 u_ViewProjection;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.Position = mul(float4(Input.a_Position, 1.0f), u_Model);
    Output.Position = mul(Output.Position, u_ViewProjection);
    Output.Normal = Input.a_Normal;
    Output.Texcoord = Input.a_Texcoord;
    return Output;
}

#type:pixel
struct PixelInput
{
    float4 Position : SV_Position;
    float3 Normal : Nor;
    float2 Texcoord : Tex;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
    int EntityID : SV_Target1;
};

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    Output.Color = float4(0.7f, 0.8f, 0.5f, 1.0f);
    return Output;
}