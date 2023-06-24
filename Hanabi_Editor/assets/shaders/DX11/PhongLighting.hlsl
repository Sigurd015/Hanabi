// --------------------------
// Phong Lighting Shader
// --------------------------

#type:vertex
struct VertexInput
{
    float3 a_Position : a_Position;
    float3 a_Normal : a_Normal;
    float2 a_TexCoord : a_TexCoord;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 LocalPosition : LocalPos;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float3 AmbientColor : Amb;
    float AmbientIntensity : AmbInt;
    float3 DirectionalLightDirection : DirDir;
    float3 DirectionalLightColor : DirColor;
    float DirectionalLightIntensity : DirInt;
    float3 CameraPosition : CamPos;
};

cbuffer ScnenData : register(b0)
{
    float4x4 u_ViewProjection;
    float3 u_CameraPosition;

    float u_AmbientIntensity;
    float3 u_AmbientColor;

    // DirectionalLight
    float u_DirectionalLightIntensity;
    float3 u_DirectionalLightDirection;
    float3 u_DirectionalLightColor;
};

cbuffer ModelData : register(b1)
{
    float4x4 u_Model;
};

float3 CalcLocal(float4x4 modelTrans, float3 vec)
{
    float3x3 worldToLocal = (float3x3) transpose(modelTrans); // Assuming the modelTrans is column-major
    float3 local = mul(worldToLocal, vec);
    return local;
}

VertexOutput main(VertexInput Input)
{
    VertexOutput Output;
    Output.LocalPosition = Input.a_Position;
    Output.Position = mul(u_Model, float4(Input.a_Position, 1.0f));
    Output.Position = mul(u_ViewProjection, Output.Position);
    Output.Normal = Input.a_Normal;
    Output.TexCoord = Input.a_TexCoord;
    Output.AmbientColor = u_AmbientColor;
    Output.AmbientIntensity = u_AmbientIntensity;
    Output.DirectionalLightDirection = CalcLocal(u_Model, u_DirectionalLightDirection);
    Output.DirectionalLightColor = u_DirectionalLightColor;
    Output.DirectionalLightIntensity = u_DirectionalLightIntensity;
    Output.CameraPosition = CalcLocal(u_Model, u_CameraPosition);
    return Output;
}

#type:pixel
struct PixelInput
{
    float4 Position : SV_Position;
    float3 LocalPosition : LocalPos;
    float3 Normal : Nor;
    float2 TexCoord : Tex;
    float3 AmbientColor : Amb;
    float AmbientIntensity : AmbInt;
    float3 DirectionalLightDirection : DirDir;
    float3 DirectionalLightColor : DirColor;
    float DirectionalLightIntensity : DirInt;
    float3 CameraPosition : CamPos;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D u_Diffuse : register(t0);
SamplerState u_DiffuseSamplerState : register(s0);

Texture2D u_Specular : register(t1);
SamplerState u_SpecularSamplerState : register(s1);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    float3 diffuse = u_Diffuse.Sample(u_DiffuseSamplerState, Input.TexCoord);
    float3 specular = u_Specular.Sample(u_SpecularSamplerState, Input.TexCoord);
    float3 ambientColor = Input.AmbientIntensity * Input.AmbientColor * diffuse;
    
    float diffuseFactor = max(dot(normalize(Input.Normal), -normalize(Input.DirectionalLightDirection)), 0);
    float3 diffuseColor = diffuseFactor * Input.DirectionalLightIntensity * Input.DirectionalLightColor * diffuse;
       
    float3 PixelToCamera = normalize(Input.CameraPosition - Input.LocalPosition);
    float3 LightReflect = normalize(reflect(Input.DirectionalLightDirection, Input.Normal));
    float SpecularFactor = max(dot(PixelToCamera, LightReflect), 0);
    float3 specularColor = SpecularFactor * Input.DirectionalLightIntensity * specular;
  
    Output.Color = float4(saturate(ambientColor + diffuseColor + specularColor), 1.0f);
    return Output;
}