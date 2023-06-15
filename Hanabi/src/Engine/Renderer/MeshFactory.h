#pragma once
#include "Mesh.h"

namespace Hanabi
{
	class MeshFactory
	{
	public:
		static Ref<StaticMesh> CreateBox(const glm::vec3& size);
		static Ref<StaticMesh> CreateSphere(float radius);
		static Ref<StaticMesh> CreateCapsule(float radius, float height);
	};

}