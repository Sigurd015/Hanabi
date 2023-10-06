#include "ViewportPanel.h"
#include "EditorResources.h"
#include "CommonStates/SelectionManager.h"
#include "CommonStates/SettingManager.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hanabi
{
	ViewPortPanel::ViewPortPanel()
	{
		m_EditorCamera = EditorCamera(30.0f, 1920 / 1080, 0.1f, 1000.0f);
	}

	ViewPortPanel::ViewPortPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void ViewPortPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
		m_ViewPortImage = SceneRenderer::GetFinalPass()->GetOutput();
	}

	void ViewPortPanel::OnEvent(Event& e)
	{
		if (m_SceneState == SceneState::Edit)
		{
			m_EditorCamera.OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(HNB_BIND_EVENT_FN(ViewPortPanel::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(HNB_BIND_EVENT_FN(ViewPortPanel::OnMouseButtonPressed));
	}

	bool ViewPortPanel::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);

		switch (e.GetKeyCode())
		{
			// Scene Commands
		case Key::D:
		{
			if (control)
				OnDuplicateEntity();
		}

		// Gizmos
		case Key::Q:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = -1;
			break;
		case Key::W:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		case Key::R:
			if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
		return false;
	}

	bool ViewPortPanel::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				SelectionManager::SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void ViewPortPanel::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = SelectionManager::GetSelectedEntity();
		if (selectedEntity)
		{
			Entity newEntity = m_Context->DuplicateEntity(selectedEntity);
			SelectionManager::SetSelectedEntity(newEntity);
		}
	}

	void ViewPortPanel::OnUpdate(Timestep ts)
	{
		m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		m_Context->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		SceneRenderer::SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		// Update scene
		switch (m_SceneState)
		{
		case SceneState::Edit:
		{
			m_EditorCamera.OnUpdate(ts);
			m_Context->OnUpdateEditor(ts, m_EditorCamera, SelectionManager::GetSelectedEntity(), SettingManager::ShowPhysicsColliders);
			break;
		}
		case SceneState::Play:
		{
			m_Context->OnUpdateRuntime(ts, SelectionManager::GetSelectedEntity(), SettingManager::ShowPhysicsColliders);
			break;
		}
		}

		switch (SettingManager::DebugDrawMode)
		{
		case SettingManager::ViewportDebugMode::None:
			m_ViewPortImage = SceneRenderer::GetFinalPass()->GetOutput();
			break;
		case SettingManager::ViewportDebugMode::GBufferAlbedo:
			m_ViewPortImage = SceneRenderer::GetGBufferAlbedo();
			break;
		case SettingManager::ViewportDebugMode::GBufferMRE:
			m_ViewPortImage = SceneRenderer::GetGBufferMRE();
			break;
		case SettingManager::ViewportDebugMode::GBufferNormal:
			m_ViewPortImage = SceneRenderer::GetGBufferNormal();
			break;
		case SettingManager::ViewportDebugMode::GBufferPosition:
			m_ViewPortImage = SceneRenderer::GetGBufferPosition();
			break;
		}

		#pragma region Mouse Pick Up
		ImVec2 mousePos = ImGui::GetMousePos();
		mousePos.x -= m_ViewportBounds[0].x;
		mousePos.y -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::OpenGL:
			mousePos.y = viewportSize.y - mousePos.y;
			break;
		}

		int mouseX = (int)mousePos.x;
		int mouseY = (int)mousePos.y;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			// TODO: Mouse Picking
		}
		#pragma endregion	
	}

	void ViewPortPanel::OnImGuiRender()
	{
		#pragma region Viewport_Toolbar	
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_Context;

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
			tintColor.w = 0.5f;

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? EditorResources::PlayIcon : EditorResources::StopIcon;
		if (ImGui::ImageButton(icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
		{
			if (m_SceneState == SceneState::Edit)
			{
				m_SceneState = SceneState::Play;
				m_OnScenePlayCallback();
			}
			else if (m_SceneState == SceneState::Play)
			{
				HNB_CORE_ASSERT(m_SceneState == SceneState::Play);

				if (m_SceneState == SceneState::Play)
					m_Context->OnRuntimeStop();

				m_SceneState = SceneState::Edit;
				m_OnSceneStopCallback();
			}
		}

		if (m_SceneState == SceneState::Play)
		{
			ImGui::SameLine();
			bool isPaused = m_Context->IsPaused();
			Ref<Texture2D> icon = isPaused ? EditorResources::PlayIcon : EditorResources::PauseIcon;
			if (ImGui::ImageButton(icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit)
					return;

				m_Context->SetPaused(!isPaused);
			}

			if (isPaused)
			{
				ImGui::SameLine();
				{
					Ref<Texture2D> icon = EditorResources::StepIcon;
					if (ImGui::ImageButton(icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
					{
						m_Context->Step();
					}
				}
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
		#pragma endregion	

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::OpenGL:
			ImGui::Image(m_ViewPortImage->GetRendererID(), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			break;
			#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			ImGui::Image(m_ViewPortImage->GetRendererID(), ImVec2{ m_ViewportSize.x, m_ViewportSize.y });
			break;
			#endif
		}

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				AssetHandle tempHandle = *(AssetHandle*)payload->Data;
				if (AssetManager::GetAssetType(tempHandle) == AssetType::Scene)
				{
					m_OnSceneOpenCallback(tempHandle);
				}
				else
				{
					//TODO: Show message to user
					HNB_CORE_ERROR("Wrong asset type!");
				}
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmos
		Entity selectedEntity = SelectionManager::GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
}