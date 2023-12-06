#include "RendererSettingPanel.h"

namespace Hanabi
{
	void RendererSettingPanel::OnEvent(Event& e)
	{}

	void RendererSettingPanel::OnUpdate(Timestep ts)
	{}

	void RendererSettingPanel::OnImGuiRender()
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
		const char* debugModeTypeStrings[] = { "None","Albedo",
			"MRE", "Normal", "Position" };
		UI::DrawComboControl("GBuffer Debug", debugModeTypeStrings, 5, DebugDrawMode);
		ImGui::End();
	}
}