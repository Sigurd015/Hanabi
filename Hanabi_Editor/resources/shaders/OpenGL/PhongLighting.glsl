// --------------------------
// 3D Static Mesh Default Shader
// --------------------------

#type:vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform SceneData
{
	mat4 u_ViewProjection;

    // DirectionalLight
    vec4 u_Ambient;
    vec3 u_Direction;
};

layout(std140, binding = 1) uniform ModelTransform
{
    mat4 u_Model;
};

struct VertexOutput
{
	vec3 Normal;
	vec2 TexCoord;
	vec4 Ambient;
};

layout (location = 0) out VertexOutput Output;

void main()
{
    Output.Normal = a_Normal;
    Output.TexCoord = a_TexCoord;
	Output.Ambient = u_Ambient;
	gl_Position =  u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type:fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput
{
	vec3 Normal;
	vec2 TexCoord;
	vec4 Ambient;
};

layout (binding = 0) uniform sampler2D u_Textures[32];

layout (location = 0) in VertexOutput Input;

void main()
{ 
	vec4 diffuseColor = texture(u_Textures[0], Input.TexCoord);
	vec4 specularColor = texture(u_Textures[1], Input.TexCoord);
    o_Color = diffuseColor + specularColor + Input.Ambient;
}