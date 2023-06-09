﻿#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Hanabi
{
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

	template<typename UIFunction, typename LoadFunction>
	static void DrawDragDropContent(UIFunction uiFunction, LoadFunction loadFunction)
	{
		uiFunction();
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* temp = (const wchar_t*)payload->Data;
				std::filesystem::path path(temp);
				loadFunction(path);
			}
			ImGui::EndDragDropTarget();
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
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f);
			}, false);

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);
				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

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

		DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_Context](auto& component) mutable
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

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				DrawDragDropContent([&component]()
					{
						if (component.Texture)
						{
							ImGui::Image(component.Texture->GetRendererID(), ImVec2(100.0f, 100.0f), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::Button("Texture", ImVec2(100.0f, 100.0f));
							ImGui::PopItemFlag();
							ImGui::PopStyleColor();
						}
					},

					[&component](auto& path)
					{
						Ref<Texture2D> texture = Texture2D::Create(path.string());
						if (texture->IsLoaded())
							component.Texture = texture;
						else
							HNB_WARN("Could not load texture {0}", path.filename().string());
					});

				ImGui::SameLine();
				if (ImGui::Button("ReSet"))
				{
					component.Texture = nullptr;
				}

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
				ImGui::DragFloat2("UV Start", glm::value_ptr(component.UVStart), 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat2("UV End", glm::value_ptr(component.UVEnd), 0.01f, 0.0f, 1.0f);
			});

		DrawComponent<MeshComponent>("Mesh", entity, [](auto& component)
			{
				DrawDragDropContent([&component]()
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						if (component.Mesh)
						{
							std::string temp = component.Mesh->GetMeshSource()->GetPath();
							ImGui::Button(temp.c_str(), ImVec2(300.0f, 30.0f));
						}
						else
						{
							ImGui::Button("Mesh", ImVec2(300.0f, 30.0f));
						}
						ImGui::PopItemFlag();
						ImGui::PopStyleColor();
					},

					[&component](auto& path)
					{
						Ref<MeshSource> meshSource = CreateRef<MeshSource>(path.string());
						Ref<Mesh> mesh = CreateRef<Mesh>(meshSource);
						component.Mesh = mesh;
					});
			});

		DrawComponent<MaterialComponent>("Material", entity, [](auto& component)
			{
				//TODO:
				if (component.Material == nullptr)
				{
					component.Material = CreateRef<MaterialAsset>();
				}

				ImGui::Text("Diffuse Texture:");
				DrawDragDropContent([&component]()
					{
						Ref<Texture2D> diffuse = component.Material->GetDiffuse();
						if (diffuse)
						{
							ImGui::Image(diffuse->GetRendererID(), ImVec2(100.0f, 100.0f), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::Button("Texture", ImVec2(100.0f, 100.0f));
							ImGui::PopItemFlag();
							ImGui::PopStyleColor();
						}
					},

					[&component](auto& path)
					{
						Ref<Texture2D> texture = Texture2D::Create(path.string());
						if (texture->IsLoaded())
							component.Material->SetDiffuse(texture);
						else
							HNB_WARN("Could not load texture {0}", path.filename().string());
					});
				ImGui::SameLine();
				if (ImGui::Button("ReSet Diffuse Texture"))
				{
					component.Material->ClearDiffuse();
				}

				ImGui::Text("Specular Texture:");
				DrawDragDropContent([&component]()
					{
						Ref<Texture2D> specular = component.Material->GetSpecular();
						if (specular)
						{
							ImGui::Image(specular->GetRendererID(), ImVec2(100.0f, 100.0f), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::Button("Texture", ImVec2(100.0f, 100.0f));
							ImGui::PopItemFlag();
							ImGui::PopStyleColor();
						}
					},

					[&component](auto& path)
					{
						Ref<Texture2D> texture = Texture2D::Create(path.string());
						if (texture->IsLoaded())
							component.Material->SetSpecular(texture);
						else
							HNB_WARN("Could not load texture {0}", path.filename().string());
					});

				ImGui::SameLine();
				if (ImGui::Button("ReSet Specular Texture"))
				{
					component.Material->ClearSpecular();
				}

				ImGui::Text("Normal Texture:");
				DrawDragDropContent([&component]()
					{
						Ref<Texture2D> normal = component.Material->GetNormal();
						if (normal)
						{
							ImGui::Image(normal->GetRendererID(), ImVec2(100.0f, 100.0f), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::Button("Texture", ImVec2(100.0f, 100.0f));
							ImGui::PopItemFlag();
							ImGui::PopStyleColor();
						}
					},

					[&component](auto& path)
					{
						Ref<Texture2D> texture = Texture2D::Create(path.string());
						if (texture->IsLoaded())
							component.Material->SetNormal(texture);
						else
							HNB_WARN("Could not load texture {0}", path.filename().string());
					});
				ImGui::SameLine();
				if (ImGui::Button("ReSet Normal Texture"))
				{
					component.Material->ClearNormal();
				}

				if (ImGui::Checkbox("Use Normal Map", &component.UseNormalMap))
				{
					component.Material->SetUseNormalMap(component.UseNormalMap);
				}
			});

		DrawComponent<LightComponent>("Light", entity, [](auto& component)
			{
				const char* lightTypeStrings[] = { "None","Directional Light","Point Light", "Spot Light" };
				const char* currentTypeString = lightTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo("Light Type", currentTypeString))
				{
					for (int i = 0; i < 4; i++)
					{
						bool isSelected = currentTypeString == lightTypeStrings[i];
						if (ImGui::Selectable(lightTypeStrings[i], isSelected))
						{
							currentTypeString = lightTypeStrings[i];
							component.Type = (LightComponent::LightType)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

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
			});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 3; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigidbody2DComponent::BodyType)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
			});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
			});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat("Radius", &component.Radius);
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<TextComponent>("Text Renderer", entity, [](auto& component)
			{
				ImGui::InputTextMultiline("Text String", &component.TextString);
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Kerning", &component.Kerning, 0.025f);
				ImGui::DragFloat("Line Spacing", &component.LineSpacing, 0.025f);
			});
	}
}