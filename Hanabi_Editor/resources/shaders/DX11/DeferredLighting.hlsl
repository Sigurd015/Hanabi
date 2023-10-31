// --------------------------
// Deferred Lighting Shader
// --------------------------

#type:vertex
#include "FullScreenQuadVertex.hlsl"

#type:pixel
#include "Buffers.hlsl"
#include "Lighting.hlsl"
#include "ShadowMappingUtils.hlsl"

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

Texture2D u_BRDFLUTTex : register(t9);
TextureCube u_EnvRadianceTex : register(t10);
TextureCube u_EnvIrradianceTex : register(t11);

SamplerState u_SSLinearWrap : register(s0);

float3 IBL(float3 F0, float3 Lr, PBRParameters params)
{
	float3 irradiance = u_EnvIrradianceTex.Sample(u_SSLinearWrap, params.Normal).rgb;
	float3 F = FresnelSchlickRoughness(F0, params.NdotV, params.Roughness);
	float3 kd = (1.0 - F) * (1.0 - params.Metalness);
	float3 diffuseIBL = params.Albedo * irradiance;

	float width, height, envRadianceTexLevels;
	u_EnvRadianceTex.GetDimensions(0, width, height, envRadianceTexLevels);
	float NoV = clamp(params.NdotV, 0.0, 1.0);
	float3 R = 2.0 * dot(params.View, params.Normal) * params.Normal - params.View;
	//float3 specularIrradiance = u_EnvRadianceTex.Sample(u_SSLinearWrap, RotateVectorAboutY(u_MaterialUniforms.EnvMapRotation, Lr), (params.Roughness) * envRadianceTexLevels).rgb;
	float3 specularIrradiance = u_EnvRadianceTex.Sample(u_SSLinearWrap, Lr, (params.Roughness) * envRadianceTexLevels).rgb;

	float2 specularBRDF = u_BRDFLUTTex.Sample(u_SSLinearWrap, float2(params.NdotV, 1.0 - params.Roughness)).rg;
	float3 specularIBL = specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);

	return kd * diffuseIBL + specularIBL;
}

// Constant normal incidence Fresnel factor for all dielectrics.
const float3 Fdielectric = float3(0.04f, 0.04f, 0.04f);

PixelOutput main(PixelInput Input)
{
    PixelOutput output;
    PBRParameters params;
    float2 texCoord = float2(Input.TexCoord.x, 1.0 - Input.TexCoord.y);
    float4 albedoTexColor = u_AlbedoBuffer.Sample(u_SSLinearWrap, texCoord);
    params.Albedo = albedoTexColor.rgb;
    float alpha = albedoTexColor.a;
    float4 MRE = u_MREBuffer.Sample(u_SSLinearWrap, texCoord);
    params.Metalness = MRE.x;
    params.Roughness = MRE.y; 
    params.Normal = u_NormalBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    params.WorldPosition = u_PositionBuffer.Sample(u_SSLinearWrap, texCoord).xyz;
    float4 lightTransformedPosition = mul(u_DirLightViewProjection, float4(params.WorldPosition, 1.0));
    
	params.View = normalize(u_CameraPosition - params.WorldPosition);
    params.NdotV = max(dot(params.Normal, params.View), 0.0);

    // Specular reflection vector
	float3 Lr = 2.0 * params.NdotV * params.Normal - params.View;

	// Fresnel reflectance, metals use albedo
	float3 F0 = lerp(Fdielectric, params.Albedo, params.Metalness);

    // Calculate direct light shadow factor
    float dirShadowFactor = 1.0f;
    switch (u_DirShadowType)
    {
        case 1:
            dirShadowFactor = CalculateHardShadow(lightTransformedPosition, params.Normal);
            break;
        case 2:
            dirShadowFactor = CalculateSoftShadow(lightTransformedPosition, params.Normal);
            break;
    }

    // Direct lighting
	float3 lightContribution = CalculateDirLights(params, F0) * dirShadowFactor;
	lightContribution += CalculatePointLights(params, F0);
	lightContribution += CalculateSpotLights(params, F0);
	lightContribution += params.Albedo * MRE.z; // MRE.z -> Emission

    // Indirect lighting
    float3 iblContribution = IBL(F0, Lr, params) * u_EnvironmentMapIntensity;

	// Final color
	output.Color = float4(iblContribution + lightContribution, 1.0f);
    return output;
}