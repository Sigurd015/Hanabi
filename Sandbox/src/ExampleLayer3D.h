#pragma once
#include <Engine/Input/CameraController.h>
#include <Hanabi.h>

class ExampleLayer3D : public Hanabi::Layer
{
public:
	Hanabi::ShaderLibrary m_ShaderLibrary;
	Hanabi::Ref<Hanabi::VertexArray> m_VertexArray;
	Hanabi::CameraController2D m_CameraController;
	glm::vec3 m_Color = { 0.2f, 0.3f, 0.8f };

	ExampleLayer3D() : Layer("ExampleLayer3D"), m_CameraController(1920.0f / 1080.0f)
	{}

	void OnAttach()override;	
	void OnDetach() override;
	void OnUpdate(Hanabi::Timestep ts) override;
	void OnEvent(Hanabi::Event& event) override;
	virtual void OnImGuiRender() override;
};