#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Events/Event.h"

namespace Hanabi
{
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Hanabi Engine", 
			uint32_t width = 1920, uint32_t height = 1080) :Title(title), Width(width), Height(height) {}
	};

	class Window
	{
	public:
		using EventCallBackFn = std::function<void(Event&)>;

		virtual ~Window() = default;
		virtual void OnUpdate() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void SetEventCallback(const EventCallBackFn& callback) = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;
		virtual void* GetNativeWindow() const = 0;
		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}