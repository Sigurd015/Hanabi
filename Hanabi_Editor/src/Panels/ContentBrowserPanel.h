#pragma once
#include "Hanabi.h"

#include <filesystem>

namespace Hanabi
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void SetProjectPath(std::filesystem::path projPath);
		void OnImGuiRender();
	private:
		void DrawDirectoryTree();
		void DrawDirectoryTree(const std::filesystem::path& directoryPath);
		void DrawContents();
		std::filesystem::path m_ProjectDirectory;
		std::filesystem::path m_SelectedDirectory;
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};
}