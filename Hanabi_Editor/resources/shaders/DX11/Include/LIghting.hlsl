#ifndef LIGHTING_HEADER
#define LIGHTING_HEADER
#include "Buffers.hlsl"
#include "PBRUtils.hlsl"

struct PBRParameters
{
	float3 Albedo;
	float Roughness;
	float Metalness;

    float3 WorldNormal;
    float3 WorldPosition;
    float3 View;  // Pixel to camera
    float NdotV;
};

float3 CalculateDirLights(PBRParameters params, float3 F0) 
{
    float3 result = float3(0, 0, 0);
    
    if (u_DirLight.Intensity == 0)
        return result;
    
    float3 Li = normalize(u_DirLight.Direction);
	float3 Lradiance = u_DirLight.Radiance * u_DirLight.Intensity;
    // half-vector
	float3 Lh = normalize(Li + params.View);

    // Calculate angles between surface normal and various light vectors.
	float cosLi = max(0.0, dot(params.WorldNormal, Li));
	float cosLh = max(0.0, dot(params.WorldNormal, Lh));

	float3 F = FresnelSchlickRoughness(F0, max(0.0, dot(Lh, params.View)), params.Roughness);
	float D = NdfGGX(cosLh, params.Roughness);
	float G = GaSchlickGGX(cosLi, params.NdotV, params.Roughness);

	float3 kd = float3(1.0f, 1.0f, 1.0f) - F * (1.0 - params.Metalness);
	float3 diffuseBRDF = kd * params.Albedo;
    
    // Cook-Torrance
	float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * params.NdotV);
	//specularBRDF = clamp(specularBRDF, float3(0, 0, 0), float3(10.0f, 10.0f, 10.0f));
	//result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    result += (diffuseBRDF / PI + specularBRDF) * Lradiance * cosLi;

    return result;
}

float3 CalculatePointLights(PBRParameters params, float3 F0)
{
    float3 result = float3(0, 0, 0);
    for (int i = 0; i < u_PointLightsCount; i++)
    {
        if (u_PointLights[i].Intensity == 0)
            continue;
        
        PointLight pointLight = u_PointLights[i];

        float3 lightDirection = pointLight.Position - params.WorldPosition;
        float3 Li = normalize(lightDirection);
		float lightDistance = length(lightDirection);
		float3 Lh = normalize(Li + params.View);
        
        float attenuation = clamp(1.0 - (lightDistance * lightDistance) / (pointLight.Radius * pointLight.Radius), 0.0, 1.0);
		attenuation *= lerp(attenuation, 1.0, pointLight.Falloff);
        
        float3 Lradiance = pointLight.Radiance * pointLight.Intensity * attenuation;

        // Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(params.WorldNormal, Li));
		float cosLh = max(0.0, dot(params.WorldNormal, Lh));

        float3 F = FresnelSchlickRoughness(F0, max(0.0, dot(Lh, params.View)), params.Roughness);
		float D = NdfGGX(cosLh, params.Roughness);
		float G = GaSchlickGGX(cosLi, params.NdotV, params.Roughness);

		float3 kd = float3(1.0f, 1.0f, 1.0f) - F * (1.0 - params.Metalness);
		float3 diffuseBRDF = kd * params.Albedo;

        // Cook-Torrance
		float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * params.NdotV);
		specularBRDF = clamp(specularBRDF, float3(0, 0, 0), float3(10.0f, 10.0f, 10.0f));
		result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }
    return result;
}

float3 CalculateSpotLights(PBRParameters params, float3 F0)
{
    float3 result = float3(0, 0, 0);
    for (int i = 0; i < u_SpotLightsCount; i++)
    {
        if (u_SpotLights[i].Intensity == 0)
            continue;
        
        SpotLight spotLight = u_SpotLights[i];

        float3 lightToWorldPos = spotLight.Position - params.WorldPosition;
        float3 Li = normalize(lightToWorldPos);
		float lightDistance = length(lightToWorldPos);
        
        float cutoff = cos(radians(spotLight.Angle * 0.5f));
		float scos = max(dot(Li, spotLight.Direction), cutoff);
		float rim = (1.0 - scos) / (1.0 - cutoff);
 
        float attenuation = clamp(1.0 - (lightDistance * lightDistance) / (spotLight.Range * spotLight.Range), 0.0, 1.0);
		attenuation *= lerp(attenuation, 1.0, spotLight.Falloff);
		attenuation *= 1.0 - pow(max(rim, 0.001), spotLight.AngleAttenuation);

        float3 Lradiance = spotLight.Radiance * spotLight.Intensity * attenuation;
		float3 Lh = normalize(Li + params.View);

        // Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(params.WorldNormal, Li));
		float cosLh = max(0.0, dot(params.WorldNormal, Lh));

        float3 F = FresnelSchlickRoughness(F0, max(0.0, dot(Lh, params.View)), params.Roughness);
		float D = NdfGGX(cosLh, params.Roughness);
		float G = GaSchlickGGX(cosLi, params.NdotV, params.Roughness);

		float3 kd = float3(1.0f, 1.0f, 1.0f) - F * (1.0 - params.Metalness);
		float3 diffuseBRDF = kd * params.Albedo;

        // Cook-Torrance
		float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * params.NdotV);
		specularBRDF = clamp(specularBRDF, float3(0, 0, 0), float3(10.0f, 10.0f, 10.0f));
		result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }
    return result;
}
#endif