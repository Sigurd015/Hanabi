#pragma once
#include "Core.h"
#include "Events/ApplicationEvent.h"
#include "Platform/Window.h"
#include "Layer/LayerStack.h"
#include "Layer/Imgui/ImGuiLayer.h"
#include "Renderer/Shader.h"
#include "Renderer/Renderer.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

namespace Hanabi
{
	class HNB_API Application
	{
	public:
		Application();
		virtual ~Application() = default;
		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		static Application* s_Instance;
	};

	Application* CreateApplication();
}