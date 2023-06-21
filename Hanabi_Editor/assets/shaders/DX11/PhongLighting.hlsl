// --------------------------
// Phong Lighting Shader
// --------------------------

#type:vertex
struct VertexInput
{
    float3 a_Position : a_Position;
    float3 a_Normal : a_Normal;
    float2 a_TexCoord : a_TexCoord;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float4 Ambient : Amb;
};

cbuffer ScnenData : register(b0)
{   
    float4x4 u_ViewProjection;
    
    // DirectionalLight
    float4 u_Ambient;
    float3 u_Direction;
};

cbuffer ModelData : register(b1)
{   
    float4x4 u_Model;
};

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.Position = mul(u_Model,float4(Input.a_Position, 1.0f));
    Output.Position = mul(u_ViewProjection,Output.Position);
    Output.Normal = Input.a_Normal;
    Output.TexCoord = Input.a_TexCoord;
    Output.Ambient = u_Ambient;
    return Output;
}

#type:pixel
struct PixelInput
{
    float4 Position : SV_Position;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float4 Ambient : Amb;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D u_Diffuse : register(t0);
SamplerState u_DiffuseSamplerState : register(s0);

Texture2D u_Specular : register(t1);
SamplerState u_SpecularSamplerState : register(s1);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    float4 diffuseColor = u_Diffuse.Sample(u_DiffuseSamplerState, Input.TexCoord);
    float4 specularColor = u_Diffuse.Sample(u_DiffuseSamplerState, Input.TexCoord);
    Output.Color = Input.Ambient + diffuseColor + specularColor;
    return Output;
}