#include "SceneHierarchyPanel.h"
#include "CommonStates/SelectionManager.h"
#include "Utils/UICore.h"

namespace Hanabi
{
	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
		Entity selectedEntity = SelectionManager::GetSelectedEntity();
		if (!selectedEntity.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				selectedEntity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnEvent(Event& e)
	{}

	void SceneHierarchyPanel::OnUpdate(Timestep ts)
	{}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID , m_Context.get() };
					if (entity.GetParentUUID() == 0)
						DrawEntityNode(entity);
				});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				SelectionManager::SetSelectedEntity({});

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
		Entity selectedEntity = SelectionManager::GetSelectedEntity();
		if (selectedEntity)
		{
			DrawComponents(selectedEntity);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		Entity selectedEntity = SelectionManager::GetSelectedEntity();
		std::vector<UUID>& children = entity.GetChildren();

		ImGuiTreeNodeFlags flags = ((selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		if (children.empty())
			flags |= ImGuiTreeNodeFlags_Leaf;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			SelectionManager::SetSelectedEntity(entity);
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (entity.GetParentUUID())
			{
				if (ImGui::MenuItem("Unparent"))
					m_Context->UnparentEntity(entity, true);
			}

			if (ImGui::MenuItem("Delete Entity"))
			{
				m_Context->DestroyEntity(entity);
				if (selectedEntity == entity)
					SelectionManager::SetSelectedEntity({});
			}

			if (ImGui::MenuItem("Create Empty Entity"))
			{
				Entity childEntity = m_Context->CreateEntity("Empty Entity");
				m_Context->ParentEntity(childEntity, entity);
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropSource())
		{
			Entity selectedEntity = SelectionManager::GetSelectedEntity();
			UUID entityID = entity.GetUUID();

			ImGui::SetDragDropPayload("SCENE_HIERARCHY_ENTITY", &entityID, sizeof(UUID));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ENTITY"))
			{
				UUID droppedEntityID = *(UUID*)payload->Data;
				HNB_CORE_INFO("Dropped entity with ID: {0}", droppedEntityID);

				Entity droppedEntity = m_Context->GetEntityByUUID(droppedEntityID);
				m_Context->ParentEntity(droppedEntity, entity);
			}
			ImGui::EndDragDropTarget();
		}

		if (opened)
		{
			for (auto child : children)
			{
				DrawEntityNode(m_Context->GetEntityByUUID(child));
			}
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

		UI::DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				UI::DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				UI::DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				UI::DrawVec3Control("Scale", component.Scale, 1.0f);
			}, false);

		UI::DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				SceneCamera::ProjectionType type = camera.GetProjectionType();
				SceneCamera::ProjectionType tempType = type;
				UI::DrawComboControl("Projection", projectionTypeStrings, 2, tempType);
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
			});

		UI::DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_Context](auto& component) mutable
			{
				bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

				static char buffer[64];
				strcpy_s(buffer, sizeof(buffer), component.ClassName.c_str());

				UI::ScopedStyleColor textColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f), !scriptClassExists);

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

		UI::DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				UI::DrawTextureControl("Texture", component.TextureHandle);

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
				ImGui::DragFloat2("UV Start", glm::value_ptr(component.UVStart), 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat2("UV End", glm::value_ptr(component.UVEnd), 0.01f, 0.0f, 1.0f);
			});

		UI::DrawComponent<MeshComponent>("Mesh", entity, [](auto& component)
			{
				std::string label = "None";
				if (AssetManager::IsAssetHandleValid(component.MeshSourceHandle)
					&& AssetManager::GetAssetType(component.MeshSourceHandle) == AssetType::MeshSource)
				{
					const AssetMetadata& metadata = Project::GetEditorAssetManager()->GetMetadata(component.MeshSourceHandle);
					label = metadata.FilePath.filename().string();
				}

				AssetHandle meshSourceHandle = component.MeshSourceHandle;

				UI::DrawDragDropContent(component.MeshSourceHandle, AssetType::MeshSource, [&]()
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

		UI::DrawComponent<MaterialComponent>("Material", entity, [](auto& component)
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

				UI::DrawDragDropContent(component.MaterialAssetHandle, AssetType::Material, [&]()
					{
						if (ImGui::Button(label.c_str(), ImVec2(100.0f, 0.0f)))
						{
							component.MaterialAssetHandle = 0;
							materialAsset = nullptr;
						}
					});

				if (materialAsset != nullptr)
				{
					AssetHandle tempAlbedoHandle = materialAsset->GetAlbedoTexHandle();
					AssetHandle tempMetalnessHandle = materialAsset->GetMetalnessTexHandle();
					AssetHandle tempRoughnessHandle = materialAsset->GetRoughnessTexHandle();
					AssetHandle tempNormalHandle = materialAsset->GetNormalTexHandle();

					UI::DrawTextureControl("Albedo Texture", tempAlbedoHandle, [&]()
						{
							float tempEmission = materialAsset->GetEmission();
							glm::vec3 tempAlbedo = materialAsset->GetAlbedo();

							if (ImGui::ColorEdit3("Albedo Color", glm::value_ptr(tempAlbedo)))
							{
								materialAsset->SetAlbedo(tempAlbedo);
								AssetImporter::Serialize(materialAsset);
							}
							if (ImGui::DragFloat("Emission", &tempEmission, 0.01f, 0.0f, 1.0f))
							{
								materialAsset->SetEmission(tempEmission);
								AssetImporter::Serialize(materialAsset);
							}
						});

					if (tempAlbedoHandle != materialAsset->GetAlbedoTexHandle())
					{
						materialAsset->SetAlbedoTex(tempAlbedoHandle);
						AssetImporter::Serialize(materialAsset);
					}

					UI::DrawTextureControl("Metalness Texture", tempMetalnessHandle, [&]()
						{
							float tempMetalness = materialAsset->GetMetalness();
							if (ImGui::DragFloat("Metalness", &tempMetalness, 0.01f, 0.0f, 1.0f))
							{
								materialAsset->SetMetalness(tempMetalness);
								AssetImporter::Serialize(materialAsset);
							}
						});

					if (tempMetalnessHandle != materialAsset->GetMetalnessTexHandle())
					{
						materialAsset->SetMetalnessTex(tempMetalnessHandle);
						AssetImporter::Serialize(materialAsset);
					}

					UI::DrawTextureControl("Roughness Texture", tempRoughnessHandle, [&]()
						{
							float tempRoughness = materialAsset->GetRoughness();
							if (ImGui::DragFloat("Roughness", &tempRoughness, 0.01f, 0.0f, 1.0f))
							{
								materialAsset->SetRoughness(tempRoughness);
								AssetImporter::Serialize(materialAsset);
							}
						});

					if (tempRoughnessHandle != materialAsset->GetRoughnessTexHandle())
					{
						materialAsset->SetRoughnessTex(tempRoughnessHandle);
						AssetImporter::Serialize(materialAsset);
					}

					UI::DrawTextureControl("Normal Texture", tempNormalHandle, [&]()
						{
							bool useNormalMap = materialAsset->IsUsingNormalMap();
							if (ImGui::Checkbox("Use Normal Map", &useNormalMap))
							{
								materialAsset->SetUseNormalMap(useNormalMap);
								AssetImporter::Serialize(materialAsset);
							}
						});

					if (tempNormalHandle != materialAsset->GetNormalTexHandle())
					{
						materialAsset->SetNormalTex(tempNormalHandle);
						AssetImporter::Serialize(materialAsset);
					}
				}
			});

		UI::DrawComponent<SkyLightComponent>("SkyLight", entity, [](auto& component)
			{
				ImGui::DragFloat("Intensity", &component.Intensity, 0.1f, 0.0f, 5.0f);

				Ref<EnvMapAsset> envMapAsset = nullptr;
				std::string label = "None";
				if (AssetManager::IsAssetHandleValid(component.EnvMapHandle)
					&& AssetManager::GetAssetType(component.EnvMapHandle) == AssetType::EnvMap)
				{
					envMapAsset = AssetManager::GetAsset<EnvMapAsset>(component.EnvMapHandle);
					const AssetMetadata& metadata = Project::GetEditorAssetManager()->GetMetadata(component.EnvMapHandle);
					label = metadata.FilePath.filename().string();
				}

				UI::DrawDragDropContent(component.EnvMapHandle, AssetType::EnvMap, [&]()
					{
						if (ImGui::Button(label.c_str(), ImVec2(100.0f, 0.0f)))
						{
							component.EnvMapHandle = 0;
							envMapAsset = nullptr;
						}
					});
			});

		UI::DrawComponent<LightComponent>("Light", entity, [](auto& component)
			{
				const char* lightTypeStrings[] = { "None","Directional Light","Point Light", "Spot Light" };
				UI::DrawComboControl("Light Type", lightTypeStrings, 4, component.Type);

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
				UI::DrawComboControl("Shadow Type", shadowTypeStrings, 3, component.Shadow);
			});

		UI::DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				UI::DrawComboControl("Body Type", bodyTypeStrings, 3, component.Type);

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
				ImGui::DragFloat("Gravity Scale", &component.GravityScale, 0.01f, -1.0f, 1.0f);
			});

		UI::DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		UI::DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
			});

		UI::DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat("Radius", &component.Radius);
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		UI::DrawComponent<TextComponent>("Text Renderer", entity, [](auto& component)
			{
				ImGui::InputTextMultiline("Text String", &component.TextString);
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Kerning", &component.Kerning, 0.025f);
				ImGui::DragFloat("Line Spacing", &component.LineSpacing, 0.025f);
			});
	}
}