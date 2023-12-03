#pragma once
#include "Hanabi.h"
#include "PanelManager.h"

#include <filesystem>

namespace Hanabi
{
	class ContentBrowserPanel : public Panel
	{
	public:
		ContentBrowserPanel() = default;
		ContentBrowserPanel(const std::filesystem::path& projectDirectory);
		void SetContext(const std::filesystem::path& projectDirectory);
		virtual void OnEvent(Event& e) override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;

		AssetHandle ImportAsset(const std::filesystem::path& path);
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