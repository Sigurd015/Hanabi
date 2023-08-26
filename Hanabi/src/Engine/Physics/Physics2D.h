#pragma once
#include "Engine/Scene/Components.h"

#include <Physix2D.h>

namespace Hanabi
{
	namespace Utils
	{
		inline Physix2D::RigidbodyType Rigidbody2DTypeToPhysix2DBody(Rigidbody2DComponent::BodyType bodyType)
		{
			switch (bodyType)
			{
			case Rigidbody2DComponent::BodyType::Static:    return Physix2D::RigidbodyType::Static;
			case Rigidbody2DComponent::BodyType::Dynamic:   return Physix2D::RigidbodyType::Dynamic;
			case Rigidbody2DComponent::BodyType::Kinematic: return Physix2D::RigidbodyType::Kinematic;
			}

			HNB_CORE_ASSERT(false, "Unknown body type");
			return Physix2D::RigidbodyType::Static;
		}

		inline Rigidbody2DComponent::BodyType Rigidbody2DTypeFromPhysix2DBody(Physix2D::RigidbodyType bodyType)
		{
			switch (bodyType)
			{
			case Physix2D::RigidbodyType::Static:    return Rigidbody2DComponent::BodyType::Static;
			case Physix2D::RigidbodyType::Dynamic:   return Rigidbody2DComponent::BodyType::Dynamic;
			case Physix2D::RigidbodyType::Kinematic: return Rigidbody2DComponent::BodyType::Kinematic;
			}

			HNB_CORE_ASSERT(false, "Unknown body type");
			return Rigidbody2DComponent::BodyType::Static;
		}
	}
}