#pragma once
#include "Hanabi.h"

struct SceneData
{
	glm::mat4 ViewProj;

	// DirectionalLight
	glm::vec4 AmbientColor;
	glm::vec3 Direction;
	float Padding;
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
	Hanabi::Ref<Hanabi::ConstantBuffer> m_SceneDataBuffer;

	Hanabi::Ref<Hanabi::Texture2D> m_DiffuseTexture;
	Hanabi::Ref<Hanabi::Texture2D> m_SpecularTexture;

	Hanabi::Ref<Hanabi::Mesh> m_Mesh;
	Hanabi::Ref<Hanabi::RenderPass> m_RenderPass;
	Hanabi::Ref<Hanabi::Pipeline> m_Pipeline;
	Hanabi::Ref<Hanabi::Material> m_Material;
	SceneData* m_SceneData;
};