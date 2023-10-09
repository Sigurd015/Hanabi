// --------------------------
// Deferred Lighting Shader
// --------------------------

#type:vertex
#include "FullScreenQuadVertex.hlsl"

#type:pixel
#include "Buffers.hlsl"
#include "Lighting.hlsl"

struct PixelInput
{
    float4 Position : SV_Position;
    float2 TexCoord : Tex;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D u_AlbedoBuffer : register(t3);
Texture2D u_MREBuffer : register(t4);
Texture2D u_NormalBuffer : register(t5);
Texture2D u_PositionBuffer : register(t6);

Texture2D u_BRDFLut : register(t9);

SamplerState u_SSLinearWrap : register(s0);

// Constant normal incidence Fresnel factor for all dielectrics.
const float3 Fdielectric = float3(0.04f, 0.04f, 0.04f);

PixelOutput main(PixelInput Input)
{
    PixelOutput output;
    PBRParameters params;
    float2 texCoord = float2(Input.TexCoord.x, 1.0 - Input.TexCoord.y);
    params.Albedo = u_AlbedoBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    float4 MRE = u_MREBuffer.Sample(u_SSLinearWrap, texCoord);
    params.Metalness = MRE.x;
    params.Roughness = max(MRE.y, 0.05); // Minimum roughness of 0.05 to keep specular highlight
    params.WorldNormal = u_NormalBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    params.WorldPosition = u_PositionBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    
	params.View = normalize(u_CameraPosition - params.WorldPosition);
    params.NdotV = max(dot(params.WorldNormal, params.View), 0.0);

    // Specular reflection vector
	float3 Lr = 2.0 * params.NdotV * params.WorldNormal - params.View;

	// Fresnel reflectance, metals use albedo
	float3 F0 = lerp(Fdielectric, params.Albedo, params.Metalness);

    // Direct lighting
	float3 lightContribution = CalculateDirLights(params, F0);
	lightContribution += CalculatePointLights(params, F0);
	lightContribution += CalculateSpotLights(params, F0);
	lightContribution += params.Albedo * MRE.z; // MRE.z -> Emission

    // Indirect lighting (TODO: Implement IBL)
    float3 iblContribution = u_SkyLightIntensity * float3(1.0f, 1.0f, 1.0f) * params.Albedo;

	// Final color
	output.Color = float4(iblContribution + lightContribution, 1.0);
    return output;
}