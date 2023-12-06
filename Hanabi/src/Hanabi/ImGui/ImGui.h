#pragma once
#include "Hanabi/Scene/Entity.h"
#include "Hanabi/Asset/AssetManager/AssetManager.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Hanabi
{
	namespace UI
	{
		class ScopedID
		{
		public:
			ScopedID(const ScopedID&) = delete;
			ScopedID& operator=(const ScopedID&) = delete;
			template<typename T>
			ScopedID(T id)
			{
				ImGui::PushID(id);
			}
			~ScopedID()
			{
				ImGui::PopID();
			}
		};

		class ScopedItemFlags
		{
		public:
			ScopedItemFlags(const ScopedItemFlags&) = delete;
			ScopedItemFlags& operator=(const ScopedItemFlags&) = delete;
			ScopedItemFlags(const ImGuiItemFlags flags, const bool enable = true)
			{
				ImGui::PushItemFlag(flags, enable);
			}
			~ScopedItemFlags() { ImGui::PopItemFlag(); }
		};

		class ScopedStyle
		{
		public:
			ScopedStyle(const ScopedStyle&) = delete;
			ScopedStyle& operator=(const ScopedStyle&) = delete;
			template<typename T>
			ScopedStyle(ImGuiStyleVar styleVar, T value)
			{
				ImGui::PushStyleVar(styleVar, value);
			}
			~ScopedStyle()
			{
				ImGui::PopStyleVar();
			}
		};

		class ScopedFont
		{
		public:
			ScopedFont(const ScopedFont&) = delete;
			ScopedFont& operator=(const ScopedFont&) = delete;
			ScopedFont(ImFont* font)
			{
				ImGui::PushFont(font);
			}
			~ScopedFont()
			{
				ImGui::PopFont();
			}
		};

		class ScopedStyleColor
		{
		public:
			ScopedStyleColor() = default;
			ScopedStyleColor(const ScopedStyleColor&) = delete;
			ScopedStyleColor& operator=(const ScopedStyleColor&) = delete;
			ScopedStyleColor(ImGuiCol idx, ImVec4 color, bool predicate = true)
				: m_Set(predicate)
			{
				if (predicate)
					ImGui::PushStyleColor(idx, color);
			}
			ScopedStyleColor(ImGuiCol idx, ImU32 color, bool predicate = true)
				: m_Set(predicate)
			{
				if (predicate)
					ImGui::PushStyleColor(idx, color);
			}
			~ScopedStyleColor()
			{
				if (m_Set)
					ImGui::PopStyleColor();
			}
		private:
			bool m_Set = false;
		};

		static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
		{
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ScopedID id(label.c_str());

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ScopedStyle itemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			{
				ScopedStyleColor button(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
				ScopedStyleColor buttonHovered(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
				ScopedStyleColor buttonActive(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
				{
					ScopedFont font(boldFont);
					if (ImGui::Button("X", buttonSize))
						values.x = resetValue;
				}
			}

			ImGui::SameLine();
			ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			{
				ScopedStyleColor button(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ScopedStyleColor buttonHovered(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
				ScopedStyleColor buttonActive(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				{
					ScopedFont font(boldFont);
					if (ImGui::Button("Y", buttonSize))
						values.y = resetValue;
				}
			}

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			{
				ScopedStyleColor button(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				ScopedStyleColor buttonHovered(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
				ScopedStyleColor buttonActive(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				{
					ScopedFont font(boldFont);
					if (ImGui::Button("Z", buttonSize))
						values.z = resetValue;
				}
			}

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::Columns(1);
		}

		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction, bool removable = true)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			if (entity.HasComponent<T>())
			{
				auto& component = entity.GetComponent<T>();
				ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

				bool open = false;
				float lineHeight = 0.0f;
				{
					UI::ScopedStyle framePadding(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
					ImGui::Separator();
					lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
					open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
				}
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

		static void DrawDragDropTarget(const std::string& payloadType, AssetHandle& handle, AssetType validType)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadType.c_str()))
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

		static void DrawAssetControl(const std::string& name, AssetHandle& handle,
			AssetType type = AssetType::Texture2D, const std::string& payloadType = "CONTENT_BROWSER_ITEM")
		{
			std::string label = "None";
			bool isTexture = type == AssetType::Texture2D;
			if (AssetManager::IsAssetHandleValid(handle))
			{
				if (isTexture)
				{
					Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(handle);
					ImGui::Image(texture->GetRendererID(), ImVec2(100.0f, 100.0f), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
					ImGui::SameLine();
				}
				const AssetMetadata& metadata = Project::GetEditorAssetManager()->GetMetadata(handle);
				label = metadata.FilePath.filename().string();
			}
			else if (isTexture)
			{
				{
					ScopedStyleColor button(ImGuiCol_Button, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
					ScopedItemFlags itemFlags(ImGuiItemFlags_Disabled);
					ImGui::Button(name.c_str(), ImVec2(100.0f, 100.0f));
				}
				ImGui::SameLine();
			}

			if (ImGui::Button(label.c_str(), ImVec2(100.0f, 0.0f)))
			{
				handle = 0;
			}
			DrawDragDropTarget(payloadType, handle, type);
		}

		template<typename T>
		static void DrawComboControl(const std::string& name, const char** typeStrings, uint32_t typeCount, T& type)
		{
			const char* currentTypeString = typeStrings[(uint32_t)type];
			if (ImGui::BeginCombo(name.c_str(), currentTypeString))
			{
				for (uint32_t i = 0; i < typeCount; i++)
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
}