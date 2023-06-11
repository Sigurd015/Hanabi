#pragma once
#include "Mesh.h"

namespace Hanabi
{
	class MeshFactory
	{
	public:
		static Ref<Mesh> CreateBox(const DirectX::XMFLOAT3& size);
		static Ref<Mesh> CreateSphere(float radius);
		static Ref<Mesh> CreateCapsule(float radius, float height);
	};

}