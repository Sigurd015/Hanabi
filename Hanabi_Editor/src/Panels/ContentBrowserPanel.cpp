#include "ContentBrowserPanel.h"

#include <imgui.h>
#include <stack>

namespace Hanabi
{
	namespace Utils
	{
		static bool HasChildDirectory(std::filesystem::path currentPath)
		{
			for (auto& directoryEntry : std::filesystem::directory_iterator(currentPath))
			{
				if (directoryEntry.is_directory())
					return true;
			}
			return false;
		}
	}

	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_DirectoryIcon = Texture2D::Create("resources/icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("resources/icons/ContentBrowser/FileIcon.png");
	}

	void ContentBrowserPanel::SetProjectPath(std::filesystem::path projPath)
	{
		m_ProjectDirectory = projPath;
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (!m_ProjectDirectory.empty())
		{
			ImGui::Columns(2);

			static bool setDefaultColumnWidth = true;
			if (setDefaultColumnWidth)
			{
				ImGui::SetColumnWidth(0, 240.0f);
				setDefaultColumnWidth = false;
			}

			if (ImGui::BeginChild("CONTENT_BROWSER_TREE"))
			{
				DrawDirectoryTree();
			}
			ImGui::EndChild();

			ImGui::NextColumn();

			if (ImGui::BeginChild("CONTENT_BROWSER_CONTENT"))
			{
				DrawContents();
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

	void ContentBrowserPanel::DrawDirectoryTree()
	{
		DrawDirectoryTree(m_ProjectDirectory);

		//TODO: Maybe add Engine asset directory	
		//ImGui::Separator();
	}

	void ContentBrowserPanel::DrawDirectoryTree(const std::filesystem::path& directoryPath)
	{
		const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

		ImGuiTreeNodeFlags nodeFlags = baseFlags;

		if (!m_SelectedDirectory.empty() && m_SelectedDirectory == directoryPath)
			nodeFlags |= ImGuiTreeNodeFlags_Selected;

		bool canOpen = Utils::HasChildDirectory(directoryPath);
		if (!canOpen)
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		std::string filenameString = directoryPath.filename().string();
		std::string label = "##" + filenameString;
		ImGui::PushID(filenameString.c_str());
		bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), nodeFlags);

		if (ImGui::IsItemClicked())
			m_SelectedDirectory = directoryPath;

		ImGui::SameLine();
		ImGui::Image(m_DirectoryIcon->GetRendererID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });
		ImGui::SameLine();
		ImGui::Text(directoryPath.filename().string().c_str());

		if (nodeOpen && canOpen)
		{
			for (auto& directoryEntry : std::filesystem::directory_iterator(directoryPath))
			{
				if (!directoryEntry.is_directory())
					continue;

				const auto& path = directoryEntry.path();
				DrawDirectoryTree(path);
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	void ContentBrowserPanel::DrawContents()
	{
		if (m_SelectedDirectory.empty())
			return;

		static float padding = 8.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_SelectedDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			Ref<Texture> Icon;
			if (directoryEntry.is_directory())
				Icon = m_DirectoryIcon;
			else
				Icon = m_FileIcon;

			ImGui::ImageButton(Icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_SelectedDirectory /= path.filename();
			}
			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns(1);
		//ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		//ImGui::SliderFloat("Padding", &padding, 0, 32);
	}
}