#ifndef MATERIAL_HEADER
#define MATERIAL_HEADER

Texture2D u_Diffuse : register(t0);
Texture2D u_Specular : register(t1);
Texture2D u_Normal : register(t2);

SamplerState u_SSLinearWrap : register(s0);

#endif