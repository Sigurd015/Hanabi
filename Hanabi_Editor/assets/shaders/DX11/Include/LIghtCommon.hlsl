#define MAX_POINT_LIGHT 2
#define MAX_SPOT_LIGHT 2

struct DirectionalLight
{
    float3 Color;
    float Intensity;
    float3 Direction;
};

struct PointLight
{
    float3 Color;
    float Intensity;
    float3 Position;
    float Constant;
    float Linear;
    float Exp;
};

struct SpotLight
{
    float3 Color;
    float Intensity;
    float3 Position;
    float Cutoff;
    float3 Direction;
    float Constant;
    float Linear;
    float Exp;
};

float3 CalcLocal(float4x4 modelTrans, float3 vec)
{
    float3x3 worldToLocal = (float3x3) transpose(modelTrans); // Assuming the modelTrans is column-major
    float3 local = mul(worldToLocal, vec);
    return local;
}

float3 CalcLightInternal(float3 color, float intensity, float3 lightDirection, float3 normal, float3 diffuseColor, float3 specularColor, float3 pixelToCamera)
{
    float3 AmbientColor = color * intensity * diffuseColor;

    float DiffuseFactor = max(dot(normal, -lightDirection), 0);

    float3 DiffuseColor = DiffuseFactor * intensity * color * specularColor;

    float3 LightReflect = normalize(reflect(lightDirection, normal));

    float SpecularFactor = max(dot(pixelToCamera, LightReflect), 0);
    float3 SpecularColor = SpecularFactor * intensity * specularColor;

    return (AmbientColor + DiffuseColor + SpecularColor);
}

float3 CalcDirectionalLight(DirectionalLight dirLight, float3 normal, float3 diffuseColor, float3 specularColor, float3 pixelToCamera)
{
    return CalcLightInternal(dirLight.Color, dirLight.Intensity, dirLight.Direction, normal, diffuseColor, specularColor, pixelToCamera);
}

float3 CalcPointLight(PointLight pointLight, float3 localPosition, float3 diffuseColor, float3 specularColor, float3 pixelToCamera)
{
    float3 LightDirection = localPosition - pointLight.Position;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    float3 Color = CalcLightInternal(pointLight.Color, pointLight.Intensity, LightDirection, localPosition, diffuseColor, specularColor, pixelToCamera);
    float Attenuation = pointLight.Constant + pointLight.Linear * Distance + pointLight.Exp * Distance * Distance;

    return Color / Attenuation;
}

float3 CalcSpotLight(SpotLight spotLight, float3 localPosition, float3 diffuseColor, float3 specularColor, float3 pixelToCamera)
{
    float3 LightToPixel = normalize(localPosition - spotLight.Position);
    float SpotFactor = dot(LightToPixel, spotLight.Direction);

    float3 Color = float3(0, 0, 0);
    if (SpotFactor > spotLight.Cutoff)
    {
        PointLight pointLight;
        pointLight.Color = spotLight.Color;
        pointLight.Intensity = spotLight.Intensity;
        pointLight.Position = spotLight.Position;
        pointLight.Constant = spotLight.Constant;
        pointLight.Linear = spotLight.Linear;
        pointLight.Exp = spotLight.Exp;
        Color = CalcPointLight(pointLight, localPosition, diffuseColor, specularColor, pixelToCamera);
        float SpotLightIntensity = (1.0 - (1.0 - SpotFactor) / (1.0 - spotLight.Cutoff));
        Color *= SpotLightIntensity;
    }
    return Color;
}