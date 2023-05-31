#pragma once
#include "Engine/Project/Project.h"

namespace Hanabi
{
	class ProjectSerializer
	{
	public:
		ProjectSerializer(Ref<Project> project) : m_Project(project) {}
		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		Ref<Project> m_Project;
	};
}