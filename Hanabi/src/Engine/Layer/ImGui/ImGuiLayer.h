#pragma once
#include "Engine/Layer/Layer.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

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
		float m_Time = 0.0f;
		bool m_BlockEvents = true;
	};
}