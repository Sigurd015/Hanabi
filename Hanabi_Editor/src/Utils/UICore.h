#pragma once
#include "Hanabi.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Hanabi::UI
{
	struct ScopedStyleColor
	{
		ScopedStyleColor() = default;

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