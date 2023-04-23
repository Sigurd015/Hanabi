#pragma once
#include <Hanabi.h>

class ExampleLayer2D : public Hanabi::Layer
{
public:
	Hanabi::Ref<Hanabi::Texture2D> m_CheckerboardTexture;
	Hanabi::Ref<Hanabi::Texture2D> m_BaseTexture;
	Hanabi::Ref<Hanabi::SubTexture2D> m_SubTexture;
	Hanabi::CameraController2D m_CameraController;
	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f,1.0f };

	ExampleLayer2D() : Layer("ExampleLayer3D"), m_CameraController(1920.0f / 1080.0f)
	{}

	void OnAttach()override;
	void OnDetach()override;
	void OnUpdate(Hanabi::Timestep ts)override;
	void ExampleLayer2D::OnImGuiRender()override;
	void ExampleLayer2D::OnEvent(Hanabi::Event& event);
};
