#include "Buffers.hlsl"
#include "PBRUtils.hlsl"

struct PBRParameters
{
	float3 Albedo;
	float Roughness;
	float Metalness;

    float3 Normal;
    float3 WorldPosition;
    float3 View;  // Pixel to camera
    float NdotV;
};
static PBRParameters m_Params;

float3 CalculateDirLights(float3 F0) 
{
    float3 result = 0;
    
    if (u_DirLight.Intensity == 0)
        return result;
    
    float3 Li = -normalize(u_DirLight.Direction);
	float3 Lradiance = u_DirLight.Radiance * u_DirLight.Intensity;
	float3 Lh = normalize(Li + m_Params.View);

    // Calculate angles between surface normal and various light vectors.
	float cosLi = max(0.0, dot(m_Params.Normal, Li));
	float cosLh = max(0.0, dot(m_Params.Normal, Lh));

	float3 F = FresnelSchlickRoughness(F0, max(0.0, dot(Lh, m_Params.View)), m_Params.Roughness);
	float D = NdfGGX(cosLh, m_Params.Roughness);
	float G = GaSchlickGGX(cosLi, m_Params.NdotV, m_Params.Roughness);

	float3 kd = (1.0f - F) * (1.0 - m_Params.Metalness);
	float3 diffuseBRDF = kd * m_Params.Albedo;
    
    // Cook-Torrance
	float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * m_Params.NdotV);
	specularBRDF = clamp(specularBRDF, 0, 10.0f);
	result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;

    return result;
}

float3 CalculatePointLights(float3 F0)
{
    float3 result = 0;

    for (int i = 0; i < u_PointLightsCount; i++)
    {
        if (u_PointLights[i].Intensity == 0)
            continue;
        
        PointLight pointLight = u_PointLights[i];

        float3 lightDirection = pointLight.Position - m_Params.WorldPosition;
        float3 Li = normalize(lightDirection);
		float lightDistance = length(lightDirection);
		float3 Lh = normalize(Li + m_Params.View);
        
        float attenuation = clamp(1.0 - (lightDistance * lightDistance) / (pointLight.Radius * pointLight.Radius), 0.0, 1.0);
		attenuation *= lerp(attenuation, 1.0, pointLight.Falloff);
        
        float3 Lradiance = pointLight.Radiance * pointLight.Intensity * attenuation;

        // Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(m_Params.Normal, Li));
		float cosLh = max(0.0, dot(m_Params.Normal, Lh));

        float3 F = FresnelSchlickRoughness(F0, max(0.0, dot(Lh, m_Params.View)), m_Params.Roughness);
		float D = NdfGGX(cosLh, m_Params.Roughness);
		float G = GaSchlickGGX(cosLi, m_Params.NdotV, m_Params.Roughness);

		float3 kd = (1.0f - F) * (1.0 - m_Params.Metalness);
		float3 diffuseBRDF = kd * m_Params.Albedo;

        // Cook-Torrance
		float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * m_Params.NdotV);
		specularBRDF = clamp(specularBRDF, 0, 10.0f);
		result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }
    return result;
}

float3 CalculateSpotLights(float3 F0)
{
    float3 result = 0;

    for (int i = 0; i < u_SpotLightsCount; i++)
    {
        if (u_SpotLights[i].Intensity == 0)
            continue;
        
        SpotLight spotLight = u_SpotLights[i];

        float3 lightToWorldPos = spotLight.Position - m_Params.WorldPosition;
        float3 Li = normalize(lightToWorldPos);
		float lightDistance = length(lightToWorldPos);
        
        float cutoff = cos(radians(spotLight.Angle * 0.5f));
		float scos = max(dot(Li, spotLight.Direction), cutoff);
		float rim = (1.0 - scos) / (1.0 - cutoff);
 
        float attenuation = clamp(1.0 - (lightDistance * lightDistance) / (spotLight.Range * spotLight.Range), 0.0, 1.0);
		attenuation *= lerp(attenuation, 1.0, spotLight.Falloff);
		attenuation *= 1.0 - pow(max(rim, 0.001), spotLight.AngleAttenuation);

        float3 Lradiance = spotLight.Radiance * spotLight.Intensity * attenuation;
		float3 Lh = normalize(Li + m_Params.View);

        // Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(m_Params.Normal, Li));
		float cosLh = max(0.0, dot(m_Params.Normal, Lh));

        float3 F = FresnelSchlickRoughness(F0, max(0.0, dot(Lh, m_Params.View)), m_Params.Roughness);
		float D = NdfGGX(cosLh, m_Params.Roughness);
		float G = GaSchlickGGX(cosLi, m_Params.NdotV, m_Params.Roughness);

		float3 kd = (1.0f - F) * (1.0 - m_Params.Metalness);
		float3 diffuseBRDF = kd * m_Params.Albedo;

        // Cook-Torrance
		float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * m_Params.NdotV);
		specularBRDF = clamp(specularBRDF, 0, 10.0f);
		result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }
    return result;
}