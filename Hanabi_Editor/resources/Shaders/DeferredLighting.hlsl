// --------------------------
// Deferred Lighting Shader
// --------------------------

#pragma stage : compute
#include "Include/Buffers.hlsl"
#include "Include/Lighting.hlsl"
#include "Include/ShadowMappingUtils.hlsl"

Texture2D u_AlbedoBuffer : register(t3);
Texture2D u_MREBuffer : register(t4);
Texture2D u_NormalBuffer : register(t5);
Texture2D u_PositionBuffer : register(t6);

Texture2D u_BRDFLUTTex : register(t11);
TextureCube u_EnvRadianceTex : register(t12);
TextureCube u_EnvIrradianceTex : register(t13);

RWTexture2D<float4> u_OutputBuffer : register(u0);

float3 IBL(float3 F0, float3 Lr)
{
    float3 irradiance = u_EnvIrradianceTex.SampleLevel(u_SSLinearWrap, m_Params.Normal, 0).rgb;
    float3 F = FresnelSchlickRoughness(F0, m_Params.NdotV, m_Params.Roughness);
    float3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
    float3 diffuseIBL = kd * m_Params.Albedo * irradiance;

    uint width, height, envRadianceTexLevels;
    u_EnvRadianceTex.GetDimensions(0, width, height, envRadianceTexLevels);
	//float3 specularIrradiance = u_EnvRadianceTex.SampleLevel(u_SSLinearWrap, RotateVectorAboutY(u_MaterialUniforms.EnvMapRotation, Lr), m_Params.Roughness * envRadianceTexLevels).rgb;
    float3 specularIrradiance = u_EnvRadianceTex.SampleLevel(u_SSLinearWrap, Lr, m_Params.Roughness * envRadianceTexLevels).rgb;

    float2 specularBRDF = u_BRDFLUTTex.SampleLevel(u_SSPointClamp, float2(m_Params.NdotV, 1.0 - m_Params.Roughness), 0).rg;
    float3 specularIBL = specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);

    return diffuseIBL + specularIBL;
}

// Constant normal incidence Fresnel factor for all dielectrics.
const float3 Fdielectric = 0.04f;

[numthreads(32, 32, 1)]
void main(uint3 ThreadID : SV_DispatchThreadID)
{
    float4 albedoTexColor = u_AlbedoBuffer.Load(uint3(ThreadID.xy, 0));
    m_Params.Albedo = albedoTexColor.rgb;
    float alpha = albedoTexColor.a;
    float4 MRE = u_MREBuffer.Load(uint3(ThreadID.xy, 0));
    m_Params.Metalness = MRE.x;
    m_Params.Roughness = MRE.y;
    m_Params.Normal = u_NormalBuffer.Load(uint3(ThreadID.xy, 0)).xyz;
    m_Params.WorldPosition = u_PositionBuffer.Load(uint3(ThreadID.xy, 0)).xyz;
    float4 lightTransformedPosition = mul(u_DirLightViewProjection, float4(m_Params.WorldPosition, 1.0));
    
    m_Params.View = normalize(u_CameraPosition - m_Params.WorldPosition);
    m_Params.NdotV = max(dot(m_Params.Normal, m_Params.View), 0.0);

    // Specular reflection vector
    float3 Lr = 2.0 * m_Params.NdotV * m_Params.Normal - m_Params.View;

	// Fresnel reflectance, metals use albedo
    float3 F0 = lerp(Fdielectric, m_Params.Albedo, m_Params.Metalness);

    // Calculate direct light shadow factor
    float dirShadowFactor = 1.0f;
    switch (u_DirShadowType)
    {
        case 1:
            dirShadowFactor = DirHardShadow(lightTransformedPosition);
            break;
        case 2:
            dirShadowFactor = DirSoftShadow(lightTransformedPosition);
            break;
    }

    // TODO: Calculate all point light shadow factors in a single pass, 
    // and then sample them in the lighting pass
    float pointShadowFactor = 1.0f;
    switch (u_PointShadowType)
    {
        case 1:
            pointShadowFactor = PointHardShadow(m_Params.WorldPosition);
            break;
        case 2:
            pointShadowFactor = PointHardShadow(m_Params.WorldPosition);
            break;
    }

    // Direct lighting
    float3 lightContribution = CalculateDirLights(F0) * dirShadowFactor;
    lightContribution += CalculatePointLights(F0) * pointShadowFactor;
    lightContribution += CalculateSpotLights(F0);
    lightContribution += m_Params.Albedo * MRE.z; // MRE.z -> Emission

    // Indirect lighting
    float3 iblContribution = IBL(F0, Lr) * u_EnvironmentMapIntensity;

	// Final color
    u_OutputBuffer[ThreadID.xy] = float4(iblContribution + lightContribution, 1.0f);
}