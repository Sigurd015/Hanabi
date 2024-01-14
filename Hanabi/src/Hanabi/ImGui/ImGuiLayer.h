#pragma once
#include "Hanabi/Layer/Layer.h"
#include "Hanabi/Events/Event.h"

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
#ifdef HNB_ENABLE_D3D
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