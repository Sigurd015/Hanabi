#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER

Texture2D u_Diffuse : register(t0);
Texture2D u_Specular : register(t1);
Texture2D u_Normal : register(t2);
Texture2D u_ShadowDepth : register(t3);

SamplerState u_SSLinearWrap : register(s0);
#endif