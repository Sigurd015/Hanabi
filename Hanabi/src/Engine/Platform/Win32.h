#pragma once
#include "Platform.h"
#include "GLFW/glfw3.h"

namespace Hanabi
{
	class WindowsWnd :public Window
	{
	public:
		WindowsWnd(const WindowProps& props);
		virtual ~WindowsWnd();
		void OnUpdate() override;
		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }
		inline void SetEventCallBack(const EventCallBackFn& callback) override { m_Data.EventCallBack = callback; }
		void SetVSync(bool enable) override;
		bool IsVSync() const override;
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallBackFn EventCallBack;
		};
		WindowData m_Data;
	};
}