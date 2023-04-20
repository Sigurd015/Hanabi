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
		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}