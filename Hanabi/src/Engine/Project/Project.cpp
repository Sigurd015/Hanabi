#include "hnbpch.h"
#include "Engine/Project/Project.h"
#include "Engine/Project/ProjectSerializer.h"

namespace Hanabi
{
	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();

		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			project->m_ProjectDirectory = path.parent_path();
			s_ActiveProject = project;
			s_ActiveProject->s_AssetManager = CreateRef<EditorAssetManager>();
			return s_ActiveProject;
		}

		return nullptr;
	}

	bool Project::Save(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Serialize(path))
		{
			s_ActiveProject->m_ProjectDirectory = path.parent_path();
			return true;
		}

		return false;
	}
}