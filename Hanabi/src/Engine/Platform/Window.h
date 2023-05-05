#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Events/Event.h"

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
			:EventCallback(callback), Title(title), Width(width), Height(height), VSync(vSync) {}
	};

	class Window
	{
	public:
		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;
		virtual void* GetNativeWindow() const = 0;
		static Scope<Window> Create(const WindowProps& props);
	};
}