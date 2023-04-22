#pragma once
#include "Engine/Base.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Platform/Window.h"
#include "Engine/Layer/LayerStack.h"
#include "Engine/Layer/Imgui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Hanabi
{
	class  Application
	{
	public:
		Application(const std::string& name);
		virtual ~Application();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void Close();
		Window& GetWindow() { return *m_Window; }
		static Application& Get() { return *s_Instance; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		static Application* s_Instance;
		float m_LastFrameTime = 0.0f;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}