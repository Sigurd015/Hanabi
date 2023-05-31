#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Scene/Scene.h"

#include <string>

namespace Hanabi
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene) {}
		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);
		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}