// --------------------------
// 3D Static Mesh Default Shader
// --------------------------

#type:vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Texcoord;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_Model;
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec3 Position;
	vec3 Normal;
	vec2 Texcoord;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int v_EntityID;

void main()
{
	Output.Position = a_Position;
    Output.Normal = a_Normal;
    Output.Texcoord = a_Texcoord;
	gl_Position = u_Model * u_ViewProjection * vec4(a_Position, 1.0);
}

#type:fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput
{
	vec3 Position;
	vec3 Normal;
	vec2 Texcoord;
};

layout (location = 0) in VertexOutput Input;

void main()
{ 
    o_Color = vec4(0.7f,0.8f,0.5f,1.0f);
}