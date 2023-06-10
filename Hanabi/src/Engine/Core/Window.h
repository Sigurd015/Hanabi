#pragma once
#include "Base.h"
#include "Engine/Events/Event.h"
#include "Engine/Renderer/RenderingContext.h"

#include <GLFW/glfw3.h>

namespace Hanabi
{
	using EventCallBackFn = std::function<void(Event&)>;

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		bool VSync;
		EventCallBackFn EventCallback;

		WindowProps(const EventCallBackFn& callback, const std::string& title = "Hanabi Engine",
			uint32_t width = 1920, uint32_t height = 1080, bool vSync = true)
			:EventCallback(callback), Title(title), Width(width), Height(height), VSync(vSync)
		{}
	};

	class Window
	{
	public:
		Window(const WindowProps& props);
		~Window();
		void OnUpdate();
		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }
		void SetWindowTitle(const std::string& title);
		void SetVSync(bool enable);
		bool IsVSync() const;
		void* GetNativeWindow() const { return m_Window; }
		static Scope<Window> Create(const WindowProps& props);
	private:
		virtual void Init();
		virtual void Shutdown();
		GLFWwindow* m_Window;
		Scope<RenderingContext> m_Context;
		WindowProps m_Data;
	};
}