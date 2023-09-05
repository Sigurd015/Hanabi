#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Hanabi
{
	namespace Utils
	{
		static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
		{
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", buttonSize))
				values.x = resetValue;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", buttonSize))
				values.y = resetValue;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Z", buttonSize))
				values.z = resetValue;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::Columns(1);

			ImGui::PopID();
		}

		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction, bool removable = true)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			if (entity.HasComponent<T>())
			{
				auto& component = entity.GetComponent<T>();
				ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImGui::Separator();
				bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
				ImGui::PopStyleVar();
				if (removable)
				{
					ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
					if (ImGui::Button("X", ImVec2{ lineHeight, lineHeight }))
					{
						entity.RemoveComponent<T>();
					}
				}
				if (open)
				{
					uiFunction(component);
					ImGui::TreePop();
				}
			}
		}

		template<typename UIFunction>
		static void DrawDragDropContent(AssetHandle& handle, AssetType validType, UIFunction uiFunction)
		{
			uiFunction();
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					AssetHandle tempHandle = *(AssetHandle*)payload->Data;
					if (AssetManager::GetAssetType(tempHandle) == validType)
					{
						handle = tempHandle;
					}
					else
					{
						//TODO: Show message to user
						HNB_CORE_ERROR("Wrong asset type!");
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		template<typename UIFunction = std::function<void()>>
		static void DrawTextureControl(const std::string& name, AssetHandle& handle, UIFunction uiFunction = []() {})
		{
			if (ImGui::CollapsingHeader(name.c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				DrawDragDropContent(handle, AssetType::Texture2D, [&handle]()
					{
						std::string label = "None";
						if (AssetManager::IsAssetHandleValid(handle)
							&& AssetManager::GetAssetType(handle) == AssetType::Texture2D)
						{
							Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
							//TODO: UV flip for directx 11
							ImGui::Image(texture->GetRendererID(), ImVec2(100.0f, 100.0f), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
							const AssetMetadata& metadata = Project::GetEditorAssetManager()->GetMetadata(handle);
							label = metadata.FilePath.filename().string();
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::Button("Texture", ImVec2(100.0f, 100.0f));
							ImGui::PopItemFlag();
							ImGui::PopStyleColor();
						}
						ImGui::SameLine();
						if (ImGui::Button(label.c_str(), ImVec2(100.0f, 0.0f)))
						{
							handle = 0;
						}
					});
				uiFunction();
			}
		}

		template<typename T>
		static void DrawComboControl(const std::string& name, const char** typeStrings, uint32_t typeCount, T& type)
		{
			const char* currentTypeString = typeStrings[(int)type];
			if (ImGui::BeginCombo(name.c_str(), currentTypeString))
			{
				for (int i = 0; i < typeCount; i++)
				{
					bool isSelected = currentTypeString == typeStrings[i];
					if (ImGui::Selectable(typeStrings[i], isSelected))
					{
						currentTypeString = typeStrings[i];
						type = (T)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID , m_Context.get() };
					DrawEntityNode(entity);
				});
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					m_Context->CreateEntity("Empty Entity");
				ImGui::EndPopup();
			}
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				m_Context->DestroyEntity(entity);
				if (m_SelectionContext == entity)
					m_SelectionContext = {};
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			//TODO: Draw Children Entity
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<ScriptComponent>("Script");
			DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
			DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
			DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
			DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
			DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");
			DisplayAddComponentEntry<TextComponent>("Text Component");
			DisplayAddComponentEntry<MeshComponent>("Mesh Component");
			DisplayAddComponentEntry<MaterialComponent>("Material Component");
			DisplayAddComponentEntry<LightComponent>("Light Component");
			DisplayAddComponentEntry<SkyLightComponent>("SkyLight Component");
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		Utils::DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				Utils::DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				Utils::DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				Utils::DrawVec3Control("Scale", component.Scale, 1.0f);
			}, false);

		Utils::DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				SceneCamera::ProjectionType type = camera.GetProjectionType();
				SceneCamera::ProjectionType tempType = type;
				Utils::DrawComboControl("Projection", projectionTypeStrings, 2, tempType);
				if (tempType != type)
					camera.SetProjectionType(tempType);

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

					float perspectiveNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspectiveNear))
						camera.SetPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspectiveFar))
						camera.SetPerspectiveFarClip(perspectiveFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);
					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				}

				const char* clearMethodTypeStrings[] = { "None","Soild Color", "Skybox" };
				Utils::DrawComboControl("Clear Method", clearMethodTypeStrings, 3, component.ClearType);

				if (component.ClearType == CameraComponent::ClearMethod::Soild_Color)
				{
					ImGui::ColorEdit4("Clear Color", glm::value_ptr(component.ClearColor));
				}

				if (component.ClearType == CameraComponent::ClearMethod::Skybox)
				{
					Ref<EnvMapAsset> envMapAsset = nullptr;
					std::string label = "None";
					if (AssetManager::IsAssetHandleValid(component.SkyboxHandle)
						&& AssetManager::GetAssetType(component.SkyboxHandle) == AssetType::EnvMap)
					{
						envMapAsset = AssetManager::GetAsset<EnvMapAsset>(component.SkyboxHandle);
						const AssetMetadata& metadata = Project::GetEditorAssetManager()->GetMetadata(component.SkyboxHandle);
						label = metadata.FilePath.filename().string();
					}

					Utils::DrawDragDropContent(component.SkyboxHandle, AssetType::EnvMap, [&]()
						{
							if (ImGui::Button(label.c_str(), ImVec2(100.0f, 0.0f)))
							{
								component.SkyboxHandle = 0;
								envMapAsset = nullptr;
							}
						});

					if (envMapAsset != nullptr)
					{
						AssetHandle topHandle = envMapAsset->GetTopHandle();
						AssetHandle bottomHandle = envMapAsset->GetBottomHandle();
						AssetHandle leftHandle = envMapAsset->GetLeftHandle();
						AssetHandle rightHandle = envMapAsset->GetRightHandle();
						AssetHandle frontHandle = envMapAsset->GetFrontHandle();
						AssetHandle backHandle = envMapAsset->GetBackHandle();

						AssetHandle tempTopHandle = topHandle;
						AssetHandle tempBottomHandle = bottomHandle;
						AssetHandle tempLeftHandle = leftHandle;
						AssetHandle tempRightHandle = rightHandle;
						AssetHandle tempFrontHandle = frontHandle;
						AssetHandle tempBackHandle = backHandle;

						Utils::DrawTextureControl("Top Texture", tempTopHandle);
						Utils::DrawTextureControl("Bottom Texture", tempBottomHandle);
						Utils::DrawTextureControl("Left Texture", tempLeftHandle);
						Utils::DrawTextureControl("Right Texture", tempRightHandle);
						Utils::DrawTextureControl("Front Texture", tempFrontHandle);
						Utils::DrawTextureControl("Back Texture", tempBackHandle);

						bool topChanged = tempTopHandle != topHandle;
						bool bottomChanged = tempBottomHandle != bottomHandle;
						bool leftChanged = tempLeftHandle != leftHandle;
						bool rightChanged = tempRightHandle != rightHandle;
						bool frontChanged = tempFrontHandle != frontHandle;
						bool backChanged = tempBackHandle != backHandle;

						if (topChanged)
						{
							envMapAsset->SetTopHandle(tempTopHandle);
						}

						if (bottomChanged)
						{
							envMapAsset->SetBottomHandle(tempBottomHandle);
						}

						if (leftChanged)
						{
							envMapAsset->SetLeftHandle(tempLeftHandle);
						}

						if (rightChanged)
						{
							envMapAsset->SetRightHandle(tempRightHandle);
						}

						if (frontChanged)
						{
							envMapAsset->SetFrontHandle(tempFrontHandle);
						}

						if (backChanged)
						{
							envMapAsset->SetBackHandle(tempBackHandle);
						}

						if (topChanged || bottomChanged || leftChanged || rightChanged || frontChanged || backChanged)
						{
							AssetImporter::Serialize(envMapAsset);
							envMapAsset->Invalidate();
						}
					}
				}
			});

		Utils::DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_Context](auto& component) mutable
			{
				bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

				static char buffer[64];
				strcpy_s(buffer, sizeof(buffer), component.ClassName.c_str());

				Utils::ScopedStyleColor textColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f), !scriptClassExists);

				if (ImGui::InputText("Class", buffer, sizeof(buffer)))
				{
					component.ClassName = buffer;
					return;
				}

				// Fields
				bool sceneRunning = scene->IsRunning();
				if (sceneRunning)
				{
					Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
					if (scriptInstance)
					{
						const auto& fields = scriptInstance->GetScriptClass()->GetFields();

						for (const auto& [name, field] : fields)
						{
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptInstance->GetFieldValue<float>(name);
								if (ImGui::DragFloat(name.c_str(), &data))
								{
									scriptInstance->SetFieldValue(name, data);
								}
							}
						}
					}
				}
				else if (scriptClassExists)
				{
					Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
					const auto& fields = entityClass->GetFields();

					auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
					for (const auto& [name, field] : fields)
					{
						// Field has been set in editor
						if (entityFields.find(name) != entityFields.end())
						{
							ScriptFieldInstance& scriptField = entityFields.at(name);

							// Display control to set it maybe
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptField.GetValue<float>();
								if (ImGui::DragFloat(name.c_str(), &data))
									scriptField.SetValue(data);
							}
						}
						else
						{
							// Display control to set it maybe
							if (field.Type == ScriptFieldType::Float)
							{
								float data = 0.0f;
								if (ImGui::DragFloat(name.c_str(), &data))
								{
									ScriptFieldInstance& fieldInstance = entityFields[name];
									fieldInstance.Field = field;
									fieldInstance.SetValue(data);
								}
							}
						}
					}
				}
			});

		Utils::DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				Utils::DrawTextureControl("Texture", component.TextureHandle);

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
				ImGui::DragFloat2("UV Start", glm::value_ptr(component.UVStart), 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat2("UV End", glm::value_ptr(component.UVEnd), 0.01f, 0.0f, 1.0f);
			});

		Utils::DrawComponent<MeshComponent>("Mesh", entity, [](auto& component)
			{
				std::string label = "None";
				if (AssetManager::IsAssetHandleValid(component.MeshSourceHandle)
					&& AssetManager::GetAssetType(component.MeshSourceHandle) == AssetType::MeshSource)
				{
					const AssetMetadata& metadata = Project::GetEditorAssetManager()->GetMetadata(component.MeshSourceHandle);
					label = metadata.FilePath.filename().string();
				}

				AssetHandle meshSourceHandle = component.MeshSourceHandle;

				Utils::DrawDragDropContent(component.MeshSourceHandle, AssetType::MeshSource, [&]()
					{
						if (ImGui::Button(label.c_str(), ImVec2(100.0f, 0.0f)))
						{
							component.MeshSourceHandle = 0;
						}
					});

				if (meshSourceHandle != component.MeshSourceHandle)
				{
					component.MeshHandle = 0;
				}
			});

		Utils::DrawComponent<MaterialComponent>("Material", entity, [](auto& component)
			{
				Ref<MaterialAsset> materialAsset = nullptr;
				std::string label = "None";
				if (AssetManager::IsAssetHandleValid(component.MaterialAssetHandle)
					&& AssetManager::GetAssetType(component.MaterialAssetHandle) == AssetType::Material)
				{
					materialAsset = AssetManager::GetAsset<MaterialAsset>(component.MaterialAssetHandle);
					const AssetMetadata& metadata = Project::GetEditorAssetManager()->GetMetadata(component.MaterialAssetHandle);
					label = metadata.FilePath.filename().string();
				}

				Utils::DrawDragDropContent(component.MaterialAssetHandle, AssetType::Material, [&]()
					{
						if (ImGui::Button(label.c_str(), ImVec2(100.0f, 0.0f)))
						{
							component.MaterialAssetHandle = 0;
							materialAsset = nullptr;
						}
					});

				if (materialAsset != nullptr)
				{
					AssetHandle diffuseHandle = materialAsset->GetDiffuseHandle();
					AssetHandle specularHandle = materialAsset->GetSpecularHandle();
					AssetHandle normalHandle = materialAsset->GetNormalHandle();

					AssetHandle tempDiffuseHandle = diffuseHandle;
					AssetHandle tempSpecularHandle = specularHandle;
					AssetHandle tempNormalHandle = normalHandle;
					bool tempUseNormalMap = materialAsset->IsUsingNormalMap();

					Utils::DrawTextureControl("Diffuse Texture", tempDiffuseHandle);
					Utils::DrawTextureControl("Specular Texture", tempSpecularHandle);
					Utils::DrawTextureControl("Normal Texture", tempNormalHandle, [&]()
						{
							ImGui::SameLine();
							bool useNormalMap = materialAsset->IsUsingNormalMap();
							if (ImGui::Checkbox("Use Normal Map", &useNormalMap))
							{
								materialAsset->SetUseNormalMap(useNormalMap);
							}
						});

					bool diffuseChanged = tempDiffuseHandle != diffuseHandle;
					bool specularChanged = tempSpecularHandle != specularHandle;
					bool normalChanged = tempNormalHandle != normalHandle;
					bool useNormalMapChanged = tempUseNormalMap != materialAsset->IsUsingNormalMap();

					if (diffuseChanged)
					{
						materialAsset->SetDiffuse(tempDiffuseHandle);
					}

					if (specularChanged)
					{
						materialAsset->SetSpecular(tempSpecularHandle);
					}

					if (normalChanged)
					{
						materialAsset->SetNormal(tempNormalHandle);
					}

					if (diffuseChanged || specularChanged || normalChanged || useNormalMapChanged)
						AssetImporter::Serialize(materialAsset);
				}
			});

		Utils::DrawComponent<SkyLightComponent>("SkyLight", entity, [](auto& component)
			{
				std::string label = "None";
				if (AssetManager::IsAssetHandleValid(component.SceneEnvironment)
					&& AssetManager::GetAssetType(component.SceneEnvironment) == AssetType::EnvMap)
				{
					const AssetMetadata& metadata = Project::GetEditorAssetManager()->GetMetadata(component.SceneEnvironment);
					label = metadata.FilePath.filename().string();
				}

				Utils::DrawDragDropContent(component.SceneEnvironment, AssetType::EnvMap, [&]()
					{
						if (ImGui::Button(label.c_str(), ImVec2(100.0f, 0.0f)))
						{
							component.SceneEnvironment = 0;
						}
					});

				ImGui::DragFloat("Intensity", &component.Intensity, 0.1f, 0.0f, 5.0f);
			});

		Utils::DrawComponent<LightComponent>("Light", entity, [](auto& component)
			{
				const char* lightTypeStrings[] = { "None","Directional Light","Point Light", "Spot Light" };
				Utils::DrawComboControl("Light Type", lightTypeStrings, 4, component.Type);

				switch (component.Type)
				{
				case LightComponent::LightType::Directional:
					ImGui::ColorEdit3("Directional Light Radiance", glm::value_ptr(component.Radiance));
					ImGui::DragFloat("Directional Light Intensity", &component.Intensity, 0.1f, 0.0f, 1000.0f);
					break;

				case LightComponent::LightType::Point:
					ImGui::ColorEdit3("Point Light Radiance", glm::value_ptr(component.Radiance));
					ImGui::DragFloat("Point Light Intensity", &component.Intensity, 0.1f, 0.0f, 1000.0f);
					ImGui::DragFloat("Point Light Radius", &component.Radius, 0.1f, 0.0f, std::numeric_limits<float>::max());
					ImGui::DragFloat("Point Light Falloff", &component.Falloff, 0.005f, 0.0f, 1.0f);
					break;

				case LightComponent::LightType::Spot:
					ImGui::ColorEdit3("Spot Light Radiance", glm::value_ptr(component.Radiance));
					ImGui::DragFloat("Spot Light Intensity", &component.Intensity, 0.1f, 0.0f, 1000.0f);
					ImGui::DragFloat("Spot Light Range", &component.Range, 0.1f, 0.0f, std::numeric_limits<float>::max());
					ImGui::DragFloat("Spot Light Angle", &component.Angle, 0.05f, 0.1f, 180.0f);
					ImGui::DragFloat("Spot Light Angle Attenuation", &component.AngleAttenuation, 0.01f, 0.0f, std::numeric_limits<float>::max());
					ImGui::DragFloat("Spot Light Falloff", &component.Falloff, 0.005f, 0.0f, 1.0f);
					break;
				}

				const char* shadowTypeStrings[] = { "None", "Hard Shadow", "Soft Shadow" };
				Utils::DrawComboControl("Shadow Type", shadowTypeStrings, 3, component.Shadow);
			});

		Utils::DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				Utils::DrawComboControl("Body Type", bodyTypeStrings, 3, component.Type);

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
				ImGui::DragFloat("Gravity Scale", &component.GravityScale, 0.01f, -1.0f, 1.0f);
			});

		Utils::DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		Utils::DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
			});

		Utils::DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat("Radius", &component.Radius);
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		Utils::DrawComponent<TextComponent>("Text Renderer", entity, [](auto& component)
			{
				ImGui::InputTextMultiline("Text String", &component.TextString);
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Kerning", &component.Kerning, 0.025f);
				ImGui::DragFloat("Line Spacing", &component.LineSpacing, 0.025f);
			});
	}
}