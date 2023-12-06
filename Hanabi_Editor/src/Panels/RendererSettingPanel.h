#pragma once
#include "PanelManager.h"

namespace Hanabi
{
	class RendererSettingPanel : public Panel
	{
	public:
		enum class ViewportDebugMode
		{
			None = 0,
			GBufferAlbedo,
			GBufferMRE,
			GBufferNormal,
			GBufferPosition,
			Count
		};
		inline static ViewportDebugMode DebugDrawMode = ViewportDebugMode::None;
		inline static bool ShowPhysicsColliders = false;
		inline static bool EnableVsyn = false;
		inline static bool ImGuiDebugWndDraw = false;

		RendererSettingPanel() = default;
		virtual void OnEvent(Event& e) override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
	};
}