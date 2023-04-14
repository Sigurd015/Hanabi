#pragma once
#include "Engine/Layer/Layer.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

namespace Hanabi
{
	class HNB_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;
		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}