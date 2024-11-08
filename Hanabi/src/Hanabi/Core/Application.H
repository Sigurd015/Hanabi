#pragma once
#include "Base.h"
#include "Hanabi/Events/ApplicationEvent.h"
#include "Window.h"
#include "Hanabi/Layer/LayerStack.h"
#include "Hanabi/Imgui/ImGuiLayer.h"
#include "Hanabi/Scripting/ScriptEngine.h"
#include "Hanabi/Renderer/RendererAPI.h"
#include "Hanabi/Renderer/RendererConfig.h"

namespace Hanabi
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			HNB_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Hanabi App";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
		bool EnableImGui;
		bool EnableScripting;
		ScriptEngineConfig ScriptConfig;
		RendererConfig RendererConfig;	
	};

	class  Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();
		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void Close();
		Window& GetWindow() { return *m_Window; }
		static Application& Get() { return *s_Instance; }
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		const ApplicationSpecification& GetSpecification() const { return m_Specification; }
		void SubmitToMainThread(const std::function<void()>& function);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		void ExecuteMainThreadQueue();

		ApplicationSpecification m_Specification;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		static Application* s_Instance;
		float m_LastFrameTime = 0.0f;
		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);
}