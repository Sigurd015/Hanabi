#pragma once

#ifdef HNB_PLATFORM_WINDOWS
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
		void SetVSync(bool enable) override;
		bool IsVSync() const override;
		void* GetNativeWindow() const override { return m_Window; }

	private:
		virtual void Init();
		virtual void Shutdown();
		GLFWwindow* m_Window;
		Scope<RenderingContext> m_Context;
		WindowProps m_Data;
	};
}
#endif