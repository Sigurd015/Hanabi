#pragma once
#include "Hanabi.h"

namespace Hanabi
{
	class SettingManager
	{
	public:
		enum class ViewportDebugMode
		{
			None = 0,
			GBufferDiffuse,
			GBufferSpecular,
			GBufferNormal,
			GBufferPosition,
			Count
		};
		inline static bool ShowPhysicsColliders = false;
		inline static bool EnableVsyn = false;
		inline static ViewportDebugMode DebugDrawMode = ViewportDebugMode::None;
		inline static bool ImGuiDebugWndDraw = false;

		static void OnImGuiRender()
		{
			EnableVsyn = Application::Get().GetWindow().IsVSync();
			ImGui::Begin("Settings");
			auto stats = Renderer2D::GetStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quads: %d", stats.QuadCount);
			ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

			ImGui::Separator();
			ImGui::Text("Render Settings:");
			ImGui::Checkbox("Show physics colliders", &ShowPhysicsColliders);
			if (ImGui::Checkbox("Enable Vsyn", &EnableVsyn))
			{
				Application::Get().GetWindow().SetVSync(EnableVsyn);
			}

			ImGui::Separator();
			ImGui::Text("GBuffer Debug:");

			const char* debugModeTypeStrings[] = { "None","GBufferDiffuse",
				"GBufferSpecular", "GBufferNormal", "GBufferPosition" };
			const char* currentTypeString = debugModeTypeStrings[(uint32_t)DebugDrawMode];
			if (ImGui::BeginCombo("Rendering Debug", currentTypeString))
			{
				for (uint32_t i = 0; i < (uint32_t)ViewportDebugMode::Count; i++)
				{
					bool isSelected = currentTypeString == debugModeTypeStrings[i];
					if (ImGui::Selectable(debugModeTypeStrings[i], isSelected))
					{
						currentTypeString = debugModeTypeStrings[i];
						DebugDrawMode = (ViewportDebugMode)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			ImGui::End();

			if (ImGuiDebugWndDraw)
				ImGui::ShowMetricsWindow();
		}
	};
}