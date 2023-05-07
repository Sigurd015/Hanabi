#pragma once
#include "Engine/Layer/Layer.h"
#include "Engine/Events/Event.h"

namespace Hanabi
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;
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
}