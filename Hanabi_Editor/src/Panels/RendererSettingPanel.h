#pragma once
#include "PanelManager.h"

namespace Hanabi
{
	class RendererSettingPanel : public Panel
	{
	public:
		inline static Ref<Image2D> ViewPortImage;
		inline static bool ShowPhysicsColliders = false;
		inline static bool EnableVsyn = false;
		inline static bool ImGuiDebugWndDraw = false;

		RendererSettingPanel();
		~RendererSettingPanel();
		virtual void OnEvent(Event& e) override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;

	private:
		void GetViewportImage();
	};
}