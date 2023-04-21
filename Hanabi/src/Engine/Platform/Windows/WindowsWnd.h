#pragma once
#include "Engine/Platform/Window.h"
#include "Engine/Renderer/RenderingContext.h"
#include <GLFW/glfw3.h>

namespace Hanabi
{
	class WindowsWnd : public Window
	{
	public:
		WindowsWnd(const WindowProps& props);
		virtual ~WindowsWnd();
		void OnUpdate() override;
		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }
		void SetEventCallback(const EventCallBackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enable) override;
		bool IsVSync() const override;
		void* GetNativeWindow() const override { return m_Window; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
		GLFWwindow* m_Window;
		Scope<RenderingContext> m_Context;
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallBackFn EventCallback;
		};
		WindowData m_Data;
	};
}