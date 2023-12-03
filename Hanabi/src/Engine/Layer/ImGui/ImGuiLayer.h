#pragma once
#include "Engine/Layer/Layer.h"
#include "Engine/Events/Event.h"

namespace Hanabi
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(const std::string& name = "Layer") :Layer(name) {}
		~ImGuiLayer() = default;
		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void Begin() = 0;
		virtual void End() = 0;
		void OnEvent(Event& e);
		void BlockEvents(bool block) { m_BlockEvents = block; }
		static ImGuiLayer* Create();
	protected:
		void Init();
		void SetDarkThemeColors();

		bool m_BlockEvents = true;
	};
	class OpenGLImGuiLayer : public ImGuiLayer
	{
	public:
		OpenGLImGuiLayer() : ImGuiLayer("ImGuiLayerOpenGL") {}
		~OpenGLImGuiLayer() = default;
		void OnAttach() override;
		void OnDetach() override;
		void Begin() override;
		void End() override;
	};
#if defined(HNB_PLATFORM_WINDOWS)
	class DX11ImGuiLayer : public ImGuiLayer
	{
	public:
		DX11ImGuiLayer() : ImGuiLayer("ImGuiLayerDX11") {}
		~DX11ImGuiLayer() = default;
		void OnAttach() override;
		void OnDetach() override;
		void Begin() override;
		void End() override;
	};
#endif
}