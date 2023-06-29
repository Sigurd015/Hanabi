#ifndef LIGHTING_HEADER
#define LIGHTING_HEADER
#include "Buffers.hlsl"

struct Material
{
    float3 DiffuseColor;
    float3 SpecularColor;
};

float3 CalcDirectionalLight(Material material, float3 worldNormal, float3 pixelToCamera)
{
    float3 result = float3(0, 0, 0);
    
    float3 lightDir = normalize(u_DirLight.Direction);
    
    float3 ambientColor = u_DirLight.Radiance * u_DirLight.Intensity * material.DiffuseColor;

    float diffuseFactor = max(dot(worldNormal, -lightDir), 0);
    float3 diffuseColor = diffuseFactor * u_DirLight.Intensity * u_DirLight.Radiance * material.SpecularColor;

    float3 lightReflect = normalize(reflect(lightDir, worldNormal));
    float specularFactor = max(dot(pixelToCamera, lightReflect), 0);
    float3 specularColor = specularFactor * u_DirLight.Intensity * material.SpecularColor;
    
    result = (ambientColor + diffuseColor + specularColor);
    return result;
}

float3 CalcPointLight(Material material, float3 worldNormal, float3 pixelToCamera, float3 worldPos)
{
    float3 result = float3(0, 0, 0);
    for (int i = 0; i < MAX_POINT_LIGHT; i++)
    {
        if (u_PointLights[i].Intensity == 0)
            continue;
        
        PointLight pointLight = u_PointLights[i];
        
        float3 lightDirection = pointLight.Position - worldPos;
        float distance = length(lightDirection);
        lightDirection = normalize(lightDirection);
      
        float3 ambientColor = pointLight.Radiance * pointLight.Intensity * material.DiffuseColor;

        float diffuseFactor = max(dot(worldNormal, -lightDirection), 0);
        float3 diffuseColor = diffuseFactor * pointLight.Intensity * pointLight.Radiance * material.SpecularColor;

        float3 lightReflect = normalize(reflect(lightDirection, worldNormal));
        float specularFactor = max(dot(pixelToCamera, lightReflect), 0);
        float3 specularColor = specularFactor * pointLight.Intensity * material.SpecularColor;

        float3 color = (ambientColor + diffuseColor + specularColor);        
        
        float attenuation = clamp(1.0 - (distance * distance) / (pointLight.Radius * pointLight.Radius), 0.0, 1.0);
        attenuation *= lerp(attenuation, 1.0, pointLight.Falloff);
        
        result += color * attenuation;
    }
    return result;
}

float3 CalcSpotLight(Material material, float3 worldNormal, float3 pixelToCamera, float3 worldPos)
{
    float3 result = float3(0, 0, 0);
    for (int i = 0; i < MAX_SPOT_LIGHT; i++)
    {
        if (u_SpotLights[i].Intensity == 0)
            continue;
        
        SpotLight spotLight = u_SpotLights[i];
        
        float3 lightToWorldPos = spotLight.Position - worldPos;
        float distance = length(lightToWorldPos);
        lightToWorldPos = normalize(lightToWorldPos);
       
        float spotFactor = dot(lightToWorldPos, normalize(spotLight.Direction));

        float3 lightDirection = normalize(spotLight.Direction);
            
        float3 ambientColor = spotLight.Radiance * spotLight.Intensity * material.DiffuseColor;

        float diffuseFactor = max(dot(worldNormal, -lightDirection), 0);
        float3 diffuseColor = diffuseFactor * spotLight.Intensity * spotLight.Radiance * material.SpecularColor;

        float3 lightReflect = normalize(reflect(lightDirection, worldNormal));
        float specularFactor = max(dot(pixelToCamera, lightReflect), 0);
        float3 specularColor = specularFactor * spotLight.Intensity * material.SpecularColor;

        float3 color = (ambientColor + diffuseColor + specularColor);
            
        float cutoff = cos(radians(spotLight.Angle * 0.5f));
        float scos = max(dot(lightToWorldPos, lightDirection), cutoff);
        float rim = (1.0 - scos) / (1.0 - cutoff);
            
        float attenuation = clamp(1.0 - (distance * distance) / (spotLight.Range * spotLight.Range), 0.0, 1.0);
        attenuation *= lerp(attenuation, 1.0, spotLight.Falloff);
        attenuation *= 1.0 - pow(max(rim, 0.001), spotLight.AngleAttenuation);
           
        result += color * attenuation;
    }
    return result;
}
#endif