#type:vertex
struct VSIn
{
    float3 pos : Position;
};
struct VSOut
{
	float4 pos : SV_Position;
	float4 color : Color;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 World; 
    float4x4 View;  
    float4x4 Proj; 
	float4 Color; 
}

VSOut main(VSIn vsi)
{
	VSOut vso;
	vso.pos = mul(float4(vsi.pos,1.0f),World);
	vso.pos = mul(vso.pos,View);
	vso.pos = mul(vso.pos,Proj);
	vso.color = Color;
	return vso;
}

#type:pixel

struct VSOut
{
	float4 pos : SV_Position;
	float4 color : Color;
};

float4 main(VSOut pin) : SV_Target
{
    return pin.color;
}