#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER

Texture2D u_Diffuse : register(t0);
SamplerState u_DiffuseSamplerState : register(s0);

Texture2D u_Specular : register(t1);
SamplerState u_SpecularSamplerState : register(s1);

Texture2D u_Normal : register(t2);
SamplerState u_NormalSamplerState : register(s2);
#endif