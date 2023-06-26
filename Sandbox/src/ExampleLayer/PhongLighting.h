#pragma once
#include "Hanabi.h"
#define MAX_POINT_LIGHT 2
#define MAX_SPOT_LIGHT 2

enum CBBingdID
{
	MODEL = 0,
	CAMERA = 1,
	SCENE = 2,
	POINT_LIGHT = 3,
	SPOT_LIGHT = 4
};

struct DirectionalLight
{
	glm::vec3 Color = { 1.0f,1.0f,1.0f };
	float Intensity = 1.0f;
	glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };

	// Padding
	float padding;
};

struct PointLight
{
	glm::vec3 Color = { 1.0f,1.0f,1.0f };
	float Intensity = 1.0f;
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	float Constant = 1.0f;
	float Linear = 0.045f;
	float Exp = 0.0075f;

	// Padding
	float padding[2];
};

struct SpotLight
{
	glm::vec3 Color = { 1.0f,1.0f,1.0f };
	float Intensity = 1.0f;
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	float Cutoff = 0.0f;
	glm::vec3 Direction = { 0.0f, 0.0f, 1.0f };
	float Constant = 1.0f;
	float Linear = 0.045f;
	float Exp = 0.0075f;

	// Padding
	float padding[2];
};

struct CBCamera
{
	glm::mat4 ViewProj;
	glm::vec3 CameraPosition;

	// Padding
	float padding;
};

struct CBModel
{
	glm::mat4 Transform;
};

struct CBScene
{
	DirectionalLight Light;
};

struct CBPointLight
{
	PointLight PointLights[MAX_POINT_LIGHT]{};
	uint32_t Count{ 0 };

	// Padding
	float padding[3];
};

struct CBSpotLight
{
	SpotLight SpotLights[MAX_SPOT_LIGHT]{};
	uint32_t Count{ 0 };

	// Padding
	float padding[3];
};

class PhongLighting :public Hanabi::Layer
{
public:
	PhongLighting();
	~PhongLighting() = default;
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Hanabi::Timestep ts) override;
	void OnEvent(Hanabi::Event& evnet) override;
	void OnImGuiRender() override;
private:
	void UI_Tool();

	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	glm::vec2 m_ViewportBounds[2];
	Hanabi::EditorCamera m_Camera;

	Hanabi::Ref<Hanabi::Framebuffer> m_Framebuffer;

	Hanabi::Ref<Hanabi::Texture2D> m_DiffuseTexture;
	Hanabi::Ref<Hanabi::Texture2D> m_SpecularTexture;

	Hanabi::Ref<Hanabi::Mesh> m_Mesh;
	Hanabi::Ref<Hanabi::RenderPass> m_RenderPass;
	Hanabi::Ref<Hanabi::Pipeline> m_Pipeline;
	Hanabi::Ref<Hanabi::Material> m_Material;

	CBCamera m_CameraData;
	CBScene m_SceneData;
	CBModel m_ModelData;
	CBPointLight m_PointLightData;
	CBSpotLight m_SpotLightData;

	Hanabi::Ref<Hanabi::ConstantBuffer> m_ModelDataBuffer;
	Hanabi::Ref<Hanabi::ConstantBuffer> m_CameraDataBuffer;
	Hanabi::Ref<Hanabi::ConstantBuffer> m_SceneDataBuffer;
	Hanabi::Ref<Hanabi::ConstantBuffer> m_PointLightDataBuffer;
	Hanabi::Ref<Hanabi::ConstantBuffer> m_SpotLightDataBuffer;

};