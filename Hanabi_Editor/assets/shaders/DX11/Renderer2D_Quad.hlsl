//--------------------------
// Renderer2D Quad Shader
// --------------------------

#type:vertex
struct VertexInput
{
	float3 a_Position : a_Position;
	float4 a_Color : a_Color;
	float2 a_TexCoord : a_TexCoord;
	int a_TexIndex : a_TexIndex;
	float a_TilingFactor : a_TilingFactor;
	int a_EntityID : a_EntityID;
};

struct VertexOutput
{
	float4 Pos : SV_Position;
	float4 Color : Cor;
	float2 TexCoord : Ted;
	int TexIndex : Tei;
	float TilingFactor : Tlf;
	int EntityID : EID;
};

cbuffer Camera : register(b0)
{
	float4x4 u_ViewProjection;
};

VertexOutput main(VertexInput Input)
{
	VertexOutput Output;
	Output.Color = Input.a_Color;
	Output.TexCoord = Input.a_TexCoord;
	Output.TexIndex = Input.a_TexIndex;
	Output.TilingFactor = Input.a_TilingFactor;
	Output.EntityID = Input.a_EntityID;
	Output.Pos = mul(float4(Input.a_Position,1.0f),u_ViewProjection);
	return Output;
}

#type:pixel
struct PixelInput
{
	float4 Pos : SV_Position;
	float4 Color : Cor;
	float2 TexCoord : Ted;
	int TexIndex : Tei;
	float TilingFactor : Tlf;
	int EntityID : EID;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
	int EntityID : SV_Target1;
};

Texture2D u_Textures0   : register(t0);
SamplerState u_SamplerState0 : register(s0);
Texture2D u_Textures1   : register(t1);
SamplerState u_SamplerState1 : register(s1);
Texture2D u_Textures2   : register(t2);
SamplerState u_SamplerState2 : register(s2);
Texture2D u_Textures3   : register(t3);
SamplerState u_SamplerState3 : register(s3);
Texture2D u_Textures4   : register(t4);
SamplerState u_SamplerState4 : register(s4);
Texture2D u_Textures5   : register(t5);
SamplerState u_SamplerState5 : register(s5);

PixelOutput main(PixelInput Input)
{
	PixelOutput Output;
	float4 texColor;
	switch (Input.TexIndex)
	{
	case 0:
		texColor = u_Textures0.Sample(u_SamplerState0, Input.TexCoord * Input.TilingFactor) * Input.Color;
		break;
	case 1:
		texColor = u_Textures1.Sample(u_SamplerState1, Input.TexCoord * Input.TilingFactor) * Input.Color;
		break;
	case 2:
		texColor = u_Textures2.Sample(u_SamplerState2, Input.TexCoord * Input.TilingFactor) * Input.Color;
		break;
	case 3:
		texColor = u_Textures3.Sample(u_SamplerState3, Input.TexCoord * Input.TilingFactor) * Input.Color;
		break;
	case 4:
        texColor = u_Textures4.Sample(u_SamplerState4, Input.TexCoord * Input.TilingFactor) * Input.Color;
        break;
    case 5:
        texColor = u_Textures5.Sample(u_SamplerState5, Input.TexCoord * Input.TilingFactor) * Input.Color;
        break;
	}

	if (texColor.a == 0.0)
		discard;
	
	Output.Color = texColor;
	Output.EntityID = Input.EntityID;
	return Output;
}
