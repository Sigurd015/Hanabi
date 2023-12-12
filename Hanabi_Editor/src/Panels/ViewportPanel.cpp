#include "ViewportPanel.h"
#include "EditorResources.h"
#include "SelectionManager.h"
#include "RendererSettingPanel.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hanabi
{
	ViewPortPanel::ViewPortPanel() : m_EditorCamera(30.0f, 1920 / 1080, 0.1f, 10000.0f) {}

	void ViewPortPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
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

	void ViewPortPanel::DrawGizmos()
	{
		Entity selectedEntity = SelectionManager::GetSelectedEntity();

		if (!selectedEntity || m_GizmoType == -1)
			return;

		glm::mat4 projectionMatrix, viewMatrix;
		if (m_SceneState == SceneState::Play)
		{
			Entity cameraEntity = m_Context->GetMainCameraEntity();
			SceneCamera& camera = cameraEntity.GetComponent<CameraComponent>();
			projectionMatrix = camera.GetProjection();
			viewMatrix = glm::inverse(m_Context->GetWorldSpaceTransformMatrix(cameraEntity));
		}
		else
		{
			projectionMatrix = m_EditorCamera.GetProjection();
			viewMatrix = m_EditorCamera.GetViewMatrix();
		}

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

		// Entity transform
		glm::mat4 transform = m_Context->GetWorldSpaceTransformMatrix(selectedEntity);
		auto& tc = selectedEntity.GetComponent<TransformComponent>();

		// Snapping
		bool snap = Input::IsKeyPressed(Key::LeftControl);
		float snapValue = 0.5f; // Snap to 0.5m for translation/scale
		// Snap to 45 degrees for rotation
		if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			snapValue = 45.0f;
		float snapValues[3] = { snapValue, snapValue, snapValue };

		if (ImGuizmo::Manipulate(glm::value_ptr(viewMatrix),
			glm::value_ptr(projectionMatrix),
			(ImGuizmo::OPERATION)m_GizmoType,
			ImGuizmo::LOCAL,
			glm::value_ptr(transform),
			nullptr, snap ? snapValues : nullptr))
		{
			// Cover to local space, if parented
			Entity parent = m_Context->TryGetEntityByUUID(selectedEntity.GetParentUUID());
			if (parent)
			{
				glm::mat4 parentTransform = m_Context->GetWorldSpaceTransformMatrix(parent);
				transform = glm::inverse(parentTransform) * transform;
			}

			glm::vec3 rotation;
			Math::DecomposeTransform(transform, tc.Translation, rotation, tc.Scale);

			glm::vec3 deltaRotation = rotation - tc.Rotation;
			tc.Rotation += deltaRotation;
		}
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

			// Camera Controls
		case Key::F:
		{
			if (SelectionManager::GetSelectedEntity())
				break;

			Entity selectedEntityID = SelectionManager::GetSelectedEntity();
			m_EditorCamera.Focus(m_Context->GetWorldSpaceTransform(selectedEntityID).Translation);
			break;
		}
		}
		return false;
	}

	bool ViewPortPanel::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == MouseButton::Left)
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
			m_Context->OnUpdateEditor(ts, m_EditorCamera, SelectionManager::GetSelectedEntity(), RendererSettingPanel::ShowPhysicsColliders);
			break;
		}
		case SceneState::Play:
		{
			m_Context->OnUpdateRuntime(ts, SelectionManager::GetSelectedEntity(), RendererSettingPanel::ShowPhysicsColliders);
			break;
		}
		}

		#pragma region Mouse Pick Up
		ImVec2 mousePos = ImGui::GetMousePos();
		mousePos.x -= m_ViewportBounds[0].x;
		mousePos.y -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

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
		{
			UI::ScopedStyle windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			UI::ScopedStyle itemInnerSpacing(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
			UI::ScopedStyleColor button(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			auto& colors = ImGui::GetStyle().Colors;
			const auto& buttonHoveredColor = colors[ImGuiCol_ButtonHovered];
			UI::ScopedStyleColor buttonHovered(ImGuiCol_ButtonHovered, ImVec4(buttonHoveredColor.x, buttonHoveredColor.y, buttonHoveredColor.z, 0.5f));
			const auto& buttonActiveColor = colors[ImGuiCol_ButtonActive];
			UI::ScopedStyleColor buttonActive(ImGuiCol_ButtonActive, ImVec4(buttonActiveColor.x, buttonActiveColor.y, buttonActiveColor.z, 0.5f));

			bool isEdit = m_SceneState == SceneState::Edit;
			bool isPlay = m_SceneState == SceneState::Play;
			const float buttonSize = 18.0f + 12.0f;
			const float edgeOffset = 4.0f;
			const float windowHeight = 32.0f; // annoying limitation of ImGui, window can't be smaller than 32 pixels
			const float numberOfButtons = 3.0f;
			const float backgroundWidth = edgeOffset * 4.0f + buttonSize * numberOfButtons;

			float toolbarX = (m_ViewportBounds[0].x + m_ViewportBounds[1].x) / 2.0f;
			ImGui::SetNextWindowPos(ImVec2(toolbarX - (backgroundWidth / 2.0f), m_ViewportBounds[0].y + edgeOffset));
			ImGui::SetNextWindowSize(ImVec2(backgroundWidth, windowHeight));
			ImGui::SetNextWindowBgAlpha(0.0f);
			ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDocking);

			bool toolbarEnabled = (bool)m_Context;

			ImVec4 tintColor = ImVec4(1, 1, 1, 1);
			if (!toolbarEnabled)
				tintColor.w = 0.5f;

			Ref<Texture2D> icon = isEdit ? EditorResources::PlayIcon : EditorResources::StopIcon;
			const float height = std::min((float)icon->GetHeight(), buttonSize);
			const float width = (float)icon->GetWidth() / (float)icon->GetHeight() * height;
			if (ImGui::ImageButton(icon->GetRendererID(), ImVec2(width, height), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (isEdit)
				{
					m_SceneState = SceneState::Play;
					m_OnScenePlayCallback();
				}
				else if (isPlay)
				{
					HNB_CORE_ASSERT(isPlay);

					if (isPlay)
						m_Context->OnRuntimeStop();

					m_SceneState = SceneState::Edit;
					m_OnSceneStopCallback();
				}
			}

			if (isPlay)
			{
				ImGui::SameLine();
				bool isPaused = m_Context->IsPaused();
				Ref<Texture2D> icon = isPaused ? EditorResources::PlayIcon : EditorResources::PauseIcon;
				if (ImGui::ImageButton(icon->GetRendererID(), ImVec2(width, height), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
				{
					if (isEdit)
						return;

					m_Context->SetPaused(!isPaused);
				}

				if (isPaused)
				{
					ImGui::SameLine();
					{
						Ref<Texture2D> icon = EditorResources::StepIcon;
						if (ImGui::ImageButton(icon->GetRendererID(), ImVec2(width, height), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
						{
							m_Context->Step();
						}
					}
				}
			}
			ImGui::End();
		}
		#pragma endregion	

		{
			UI::ScopedStyle windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Viewport");

			m_ViewportHovered = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			ImGui::Image(RendererSettingPanel::ViewPortImage->GetRendererID(), ImVec2{ m_ViewportSize.x, m_ViewportSize.y });

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
					else if (AssetManager::GetAssetType(tempHandle) == AssetType::MeshSource)
					{
						// Import mesh
						if (m_SceneState == SceneState::Play)
						{
							HNB_CORE_ERROR("Cannot import mesh while playing!");
							return;
						}

						if (AssetManager::IsAssetHandleValid(tempHandle))
						{
							auto meshSource = AssetManager::GetAsset<MeshSource>(tempHandle);

							Entity rootEntity = m_Context->InstantiateMesh(meshSource);
							SelectionManager::SetSelectedEntity(rootEntity);
						}
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
			DrawGizmos();
			ImGui::End();
		}
	}
}