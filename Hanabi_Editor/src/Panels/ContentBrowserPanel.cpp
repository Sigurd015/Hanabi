#include "ContentBrowserPanel.h"
#include "EditorResources.h"

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

	ContentBrowserPanel::ContentBrowserPanel() :m_ProjectDirectory(Project::GetProjectDirectory())
	{
		RefreshAssetsMap();
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
				if (!m_SelectedDirectory.empty())
				{
					if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
					{
						if (ImGui::MenuItem("Create Material"))
						{
							//TODO: Popup a input box to get the name of the material, then create it, then save to the disk
						}
						ImGui::EndPopup();
					}

					DrawContents();
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

	void ContentBrowserPanel::DrawDirectoryTree()
	{
		DrawDirectoryTree(m_ProjectDirectory);
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
		ImGui::Image(EditorResources::DirectoryIcon->GetRendererID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });
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

			bool isDirectory = directoryEntry.is_directory();
			bool isAsset = false;
			auto relativePath = std::filesystem::relative(path, Project::GetAssetDirectory());

			if (!isDirectory)
				isAsset = IsAlreadyImported(relativePath);

			if (isDirectory)
				Icon = EditorResources::DirectoryIcon;
			else if (isAsset)
				Icon = EditorResources::ImportedFileIcon;
			else
				Icon = EditorResources::FileIcon;

			ImGui::ImageButton(Icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (!isDirectory && !isAsset)
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Import"))
						m_ImportedFiles[relativePath] = Project::GetEditorAssetManager()->ImportAsset(relativePath);

					ImGui::EndPopup();
				}
			}

			if (isAsset)
			{
				if (ImGui::BeginDragDropSource())
				{
					AssetHandle temp = m_ImportedFiles[relativePath];
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &temp, sizeof(AssetHandle));
					ImGui::EndDragDropSource();
				}
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (isDirectory)
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

	void ContentBrowserPanel::RefreshAssetsMap()
	{
		const auto& assetRegistry = Project::GetEditorAssetManager()->GetAssetRegistry();
		for (const auto& [handle, metadata] : assetRegistry)
		{
			auto& path = metadata.FilePath;
			m_ImportedFiles[path] = handle;
		}
	}

	bool ContentBrowserPanel::IsAlreadyImported(const std::filesystem::path& path)
	{
		if (m_ImportedFiles.find(path) != m_ImportedFiles.end())
			return m_ImportedFiles.at(path);
		return false;
	}
}