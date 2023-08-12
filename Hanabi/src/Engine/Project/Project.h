#pragma once
#include "Engine/Core/Base.h"	
#include "Engine/Asset/RuntimeAssetManager.h"
#include "Engine/Asset/EditorAssetManager.h"

#include <string>
#include <filesystem>

namespace Hanabi
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path AssetRegistryPath; // Relative to AssetDirectory
		std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:
		static const std::filesystem::path& GetProjectDirectory()
		{
			HNB_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			HNB_CORE_ASSERT(s_ActiveProject);
			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetAssetRegistryPath()
		{
			HNB_CORE_ASSERT(s_ActiveProject);
			return GetAssetDirectory() / s_ActiveProject->m_Config.AssetRegistryPath;
		}

		// TODO: move to asset manager
		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			HNB_CORE_ASSERT(s_ActiveProject);
			return GetAssetDirectory() / path;
		}

		ProjectConfig& GetConfig() { return m_Config; }
		static Ref<Project> GetActive() { return s_ActiveProject; }

		std::shared_ptr<AssetManagerBase> GetAssetManager() { return m_AssetManager; }
		std::shared_ptr<RuntimeAssetManager> GetRuntimeAssetManager() { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManager); }
		std::shared_ptr<EditorAssetManager> GetEditorAssetManager() { return std::static_pointer_cast<EditorAssetManager>(m_AssetManager); }

		static Ref<Project> Load(const std::filesystem::path& path);
		static bool Save(const std::filesystem::path& path);
		static std::string& GetProjectName() { return s_ActiveProject->m_Config.Name; }
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		std::shared_ptr<AssetManagerBase> m_AssetManager;

		inline static Ref<Project> s_ActiveProject;
	};
}