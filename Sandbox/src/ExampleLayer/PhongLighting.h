#pragma once
#include "Hanabi.h"

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

	Hanabi::Ref<Hanabi::Texture2D> m_DiffuseTexture;
	Hanabi::Ref<Hanabi::Texture2D> m_SpecularTexture;

	Hanabi::Ref<Hanabi::Mesh> m_Mesh;
	Hanabi::Ref<Hanabi::MaterialAsset> m_Material;

	std::vector<glm::mat4> m_ModelMatrices;

	Hanabi::Environment m_Environment;
};