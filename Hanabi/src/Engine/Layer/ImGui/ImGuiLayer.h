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
		virtual void OnEvent(Event& e) = 0;
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void BlockEvents(bool block) { m_BlockEvents = block; }
		virtual void SetDarkThemeColors() = 0;
		static ImGuiLayer* Create();
	private:
		bool m_BlockEvents = true;
	};
	class ImGuiLayerOpenGL : public ImGuiLayer
	{
	public:
		ImGuiLayerOpenGL();
		~ImGuiLayerOpenGL() = default;
		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& e) override;
		void Begin();
		void End();
		void BlockEvents(bool block) { m_BlockEvents = block; }
		void SetDarkThemeColors();
	private:
		bool m_BlockEvents = true;
	};
#if defined(HNB_PLATFORM_WINDOWS)
	class ImGuiLayerDX11 : public ImGuiLayer
	{
	public:
		ImGuiLayerDX11();
		~ImGuiLayerDX11() = default;
		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& e) override;
		void Begin();
		void End();
		void BlockEvents(bool block) { m_BlockEvents = block; }
		void SetDarkThemeColors();
	private:
		bool m_BlockEvents = true;
	};
#endif
}