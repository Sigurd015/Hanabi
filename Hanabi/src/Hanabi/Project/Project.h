#pragma once
#include "Hanabi/Core/Base.h"	
#include "Hanabi/Asset/AssetManager/RuntimeAssetManager.h"
#include "Hanabi/Asset/AssetManager/EditorAssetManager.h"

#include <string>
#include <filesystem>

namespace Hanabi
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";

		AssetHandle StartScene;

		std::filesystem::path AssetDirectory;

		// Relative to AssetDirectory
		std::filesystem::path AssetRegistryPath;
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

		ProjectConfig& GetConfig() { return m_Config; }
		static Ref<Project> GetActive() { return s_ActiveProject; }

		inline static Ref<AssetManagerBase> GetAssetManager() { return s_AssetManager; }
		inline static Ref<RuntimeAssetManager> GetRuntimeAssetManager() { return std::static_pointer_cast<RuntimeAssetManager>(s_AssetManager); }
		inline static Ref<EditorAssetManager> GetEditorAssetManager() { return std::static_pointer_cast<EditorAssetManager>(s_AssetManager); }

		static Ref<Project> Load(const std::filesystem::path& path);
		static bool Save(const std::filesystem::path& path);
		static std::string& GetProjectName() { return s_ActiveProject->m_Config.Name; }
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;

		inline static Ref<AssetManagerBase> s_AssetManager;
		inline static Ref<Project> s_ActiveProject;
	};
}