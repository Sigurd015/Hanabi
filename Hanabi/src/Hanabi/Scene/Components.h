#pragma once
#include "Hanabi/Core/UUID.h"
#include "Hanabi/Scene/SceneCamera.h"
#include "Hanabi/Renderer/Camera.h"
#include "Hanabi/Renderer/Texture.h"
#include "Hanabi/Renderer/UI/Font.h"
#include "Hanabi/Renderer/Mesh.h"
#include "Hanabi/Renderer/Material.h"
#include "Hanabi/Renderer/MaterialAsset.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Hanabi
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct RelationshipComponent
	{
		UUID ParentHandle = 0;
		std::vector<UUID> Children;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;
		RelationshipComponent(UUID parent)
			: ParentHandle(parent)
		{}
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation) : Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation))
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::vec3 GetRotationEuler() const
		{
			return Rotation;
		}

		glm::quat GetRotation() const
		{
			return glm::quat(Rotation);
		}

		// Right hand coordinate system, -z forward, y up, x right
		glm::vec3 GetForward() const
		{
			return glm::rotate(GetRotation(), glm::vec3(0.0f, 0.0f, -1.0f));
		}

		glm::vec3 GetUp() const
		{
			return glm::rotate(GetRotation(), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		glm::vec3 GetRight() const
		{
			return glm::rotate(GetRotation(), glm::vec3(1.0f, 0.0f, 0.0f));
		}

		void SetTransform(const glm::mat4& transform)
		{
			Math::DecomposeTransform(transform, Translation, Rotation, Scale);
		}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		operator SceneCamera& () { return Camera; }
		operator const SceneCamera& () const { return Camera; }
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		AssetHandle TextureHandle = 0;
		float TilingFactor = 1.0f;
		glm::vec2 UVStart = { 0.0f, 0.0f };
		glm::vec2 UVEnd = { 1.0f,1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
	};

	struct MeshComponent
	{
		AssetHandle MeshSourceHandle = 0;
		uint32_t SubmeshIndex = 0;
		// Storage for runtime memory only asset, not need to serialize
		AssetHandle MeshHandle = 0;

		MeshComponent() = default;
		MeshComponent(const MeshComponent& other)
			: MeshSourceHandle(other.MeshSourceHandle), SubmeshIndex(other.SubmeshIndex), MeshHandle(other.MeshHandle)
		{}
		MeshComponent(AssetHandle meshSourceHandle, uint32_t submeshIndex = 0)
			: MeshSourceHandle(meshSourceHandle), SubmeshIndex(submeshIndex)
		{}
	};

	struct MaterialComponent
	{
		AssetHandle MaterialAssetHandle = 0;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
	};

	struct SkyLightComponent
	{
		AssetHandle EnvMapHandle = 0;
		float Intensity = 1.0f;
	};

	struct LightComponent
	{
		enum class LightType { None = 0, Directional, Point, Spot };
		LightType Type = LightType::None;
		glm::vec3 Radiance = { 1.0f,1.0f,1.0f };
		float Intensity = 0.0f;
		float Radius;
		float Falloff;
		float AngleAttenuation;
		float Range;
		float Angle;

		enum class ShadowType { None = 0, Hard, Soft };
		ShadowType Shadow = ShadowType::None;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	// Physics
	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;
		float GravityScale = 1.0f;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		// TODO: move into physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		// TODO: move into physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	struct TextComponent
	{
		std::string TextString;
		Ref<Font> FontAsset = Font::GetDefault();
		glm::vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;

		TextComponent() = default;
		TextComponent(const TextComponent&) = default;
	};

	template<typename... Component>
	struct ComponentGroup
	{};

	using AllComponents =
		ComponentGroup<RelationshipComponent, TransformComponent,
		SpriteRendererComponent, CircleRendererComponent,
		MeshComponent, MaterialComponent,
		LightComponent, SkyLightComponent,
		CameraComponent,
		ScriptComponent,
		Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent,
		TextComponent>;
}