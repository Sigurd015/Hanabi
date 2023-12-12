#include "RendererSettingPanel.h"

namespace Hanabi
{
	struct RendererSettingData
	{
		enum class ViewportDebugMode
		{
			None = 0,
			GBufferAlbedo,
			GBufferMRE,
			GBufferNormal,
			GBufferPosition,
			Count
		};
		ViewportDebugMode DebugDrawMode = ViewportDebugMode::None;
	};
	static RendererSettingData* s_Data;

	RendererSettingPanel::RendererSettingPanel()
	{
		s_Data = new RendererSettingData();
		GetViewportImage();
	}

	RendererSettingPanel::~RendererSettingPanel()
	{
		delete s_Data;
	}

	void RendererSettingPanel::OnEvent(Event& e)
	{}

	void RendererSettingPanel::OnUpdate(Timestep ts)
	{}

	void RendererSettingPanel::OnImGuiRender()
	{
		EnableVsyn = Application::Get().GetWindow().IsVSync();
		ImGui::Begin("Settings");
		if (ImGui::CollapsingHeader("Renderer Settings", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Checkbox("Enable Vsyn", &EnableVsyn))
			{
				Application::Get().GetWindow().SetVSync(EnableVsyn);
			}

			if (ImGui::CollapsingHeader("Renderer2D Settings", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto stats = Renderer2D::GetStats();
				ImGui::Text("Draw Calls: %d", stats.DrawCalls);
				ImGui::Text("Quads: %d", stats.QuadCount);
				ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
				ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
			}

			if (ImGui::CollapsingHeader("Physics Settings", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Show physics colliders", &ShowPhysicsColliders);
			}

			if (ImGui::CollapsingHeader("Scene Renderer Settings", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto stats = SceneRenderer::GetStats();
				ImGui::Text("Draw Calls: %d", stats.DrawCalls);

				const char* debugModeTypeStrings[] = { "None","Albedo",
				"MRE", "Normal", "Position" };
				UI::DrawComboControl("GBuffer Debug", debugModeTypeStrings, 5, s_Data->DebugDrawMode);
				GetViewportImage();

				ImGui::Text("Dir Light Shadow Map");
				ImGui::Image(SceneRenderer::GetDirShadowMap()->GetRendererID(), ImVec2(300.0f, 300.0f));
			}
		}
		ImGui::End();
	}

	void RendererSettingPanel::GetViewportImage()
	{
		switch (s_Data->DebugDrawMode)
		{
		case RendererSettingData::ViewportDebugMode::None:
			ViewPortImage = SceneRenderer::GetFinalPass()->GetOutput();
			break;
		case RendererSettingData::ViewportDebugMode::GBufferAlbedo:
			ViewPortImage = SceneRenderer::GetGBufferAlbedo();
			break;
		case RendererSettingData::ViewportDebugMode::GBufferMRE:
			ViewPortImage = SceneRenderer::GetGBufferMRE();
			break;
		case RendererSettingData::ViewportDebugMode::GBufferNormal:
			ViewPortImage = SceneRenderer::GetGBufferNormal();
			break;
		case RendererSettingData::ViewportDebugMode::GBufferPosition:
			ViewPortImage = SceneRenderer::GetGBufferPosition();
			break;
		}
	}
}