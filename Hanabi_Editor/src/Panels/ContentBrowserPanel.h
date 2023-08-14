#pragma once
#include "Hanabi.h"

#include <filesystem>

namespace Hanabi
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void OnImGuiRender();
	private:
		void DrawDirectoryTree();
		void DrawDirectoryTree(const std::filesystem::path& directoryPath);
		void DrawContents();
		void RefreshAssetsMap();
		bool IsAlreadyImported(const std::filesystem::path& path);
		std::filesystem::path m_ProjectDirectory;
		std::filesystem::path m_SelectedDirectory;
		std::unordered_map<std::filesystem::path, AssetHandle> m_ImportedFiles;
	};
}