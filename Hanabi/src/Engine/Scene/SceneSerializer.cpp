#include "hnbpch.h"
#include "Engine/Scene/SceneCamera.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Core/UUID.h"
#include "Engine/Project/Project.h"
#include "Engine/Renderer/Renderer.h"

#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<Hanabi::UUID>
	{
		static Node encode(const Hanabi::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, Hanabi::UUID& uuid)
		{
			uuid = node.as<uint64_t>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Hanabi
{

#define WRITE_SCRIPT_FIELD(FieldType, Type)           \
			case ScriptFieldType::FieldType:          \
				out << scriptField.GetValue<Type>();  \
				break

#define READ_SCRIPT_FIELD(FieldType, Type)             \
	case ScriptFieldType::FieldType:                   \
	{                                                  \
		Type data = scriptField["Data"].as<Type>();    \
		fieldInstance.SetValue(data);                  \
		break;                                         \
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	static std::string LightTypeToString(LightComponent::LightType lightType)
	{
		switch (lightType)
		{
		case LightComponent::LightType::Point:  return "Point";
		case LightComponent::LightType::Spot:   return "Spot";
		case LightComponent::LightType::Directional: return "Directional";
		}

		HNB_CORE_ASSERT(false, "Unknown body type");
		return {};
	}

	static LightComponent::LightType LightTypeFromString(const std::string& lightTypeString)
	{
		if (lightTypeString == "Point")    return LightComponent::LightType::Point;
		if (lightTypeString == "Spot")   return LightComponent::LightType::Spot;
		if (lightTypeString == "Directional") return LightComponent::LightType::Directional;

		HNB_CORE_ASSERT(false, "Unknown body type");
		return LightComponent::LightType::NONE;
	}

	static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Rigidbody2DComponent::BodyType::Static:    return "Static";
		case Rigidbody2DComponent::BodyType::Dynamic:   return "Dynamic";
		case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		HNB_CORE_ASSERT(false, "Unknown body type");
		return {};
	}

	static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static")    return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic")   return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

		HNB_CORE_ASSERT(false, "Unknown body type");
		return Rigidbody2DComponent::BodyType::Static;
	}

	template<typename T, typename Function>
	static void SerializeComponent(const std::string& name, Entity entity, YAML::Emitter& out, Function func)
	{
		if (entity.HasComponent<T>())
		{
			out << YAML::Key << name;
			out << YAML::BeginMap;
			func();
			out << YAML::EndMap;
		}
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		HNB_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		SerializeComponent<TagComponent>("TagComponent", entity, out, [&]()
			{
				auto& tag = entity.GetComponent<TagComponent>().Tag;
				out << YAML::Key << "Tag" << YAML::Value << tag;
			});

		SerializeComponent<TransformComponent>("TransformComponent", entity, out, [&]()
			{
				auto& tc = entity.GetComponent<TransformComponent>();
				out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
				out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
				out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
			});

		SerializeComponent<CameraComponent>("CameraComponent", entity, out, [&]()
			{
				auto& cameraComponent = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;

				out << YAML::Key << "Camera" << YAML::Value;
				out << YAML::BeginMap; // Camera
				out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
				out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
				out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
				out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
				out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
				out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
				out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
				out << YAML::EndMap; // Camera

				out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
				out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			});

		SerializeComponent<MeshComponent>("MeshComponent", entity, out, [&]()
			{
				auto& meshComponent = entity.GetComponent<MeshComponent>();
				auto& mesh = meshComponent.Mesh;
				if (mesh)
					out << YAML::Key << "MeshPath" << YAML::Value << mesh->GetMeshSource()->GetPath().c_str();
			});

		SerializeComponent<MaterialComponent>("MaterialComponent", entity, out, [&]()
			{
				auto& materialComponent = entity.GetComponent<MaterialComponent>();
				auto& diffuse = materialComponent.Material->GetDiffuse();
				auto& specular = materialComponent.Material->GetSpecular();
				auto& normal = materialComponent.Material->GetNormal();

				std::string diffusePath = "";
				std::string specularPath = "";
				std::string normalPath = "";

				if (diffuse)
					diffusePath = diffuse->GetPath();
				if (specular)
					specularPath = specular->GetPath();
				if (normal)
					normalPath = normal->GetPath();

				if (diffusePath != "")
					out << YAML::Key << "DiffuseTexturePath" << YAML::Value << diffusePath.c_str();

				if (specularPath != "")
					out << YAML::Key << "SpecularTexturePath" << YAML::Value << specularPath.c_str();

				if (normalPath != "")
					out << YAML::Key << "NormalTexturePath" << YAML::Value << normalPath.c_str();

				out << YAML::Key << "UseNormalMap" << YAML::Value << materialComponent.Material->IsUsingNormalMap();
			});

		SerializeComponent<LightComponent>("LightComponent", entity, out, [&]()
			{
				auto& lightComponent = entity.GetComponent<LightComponent>();
				out << YAML::Key << "LightType" << YAML::Value << LightTypeToString(lightComponent.Type);
				out << YAML::Key << "Intensity" << YAML::Value << lightComponent.Intensity;
				out << YAML::Key << "Radiance" << YAML::Value << lightComponent.Radiance;
				out << YAML::Key << "Radius" << YAML::Value << lightComponent.Radius;
				out << YAML::Key << "Range" << YAML::Value << lightComponent.Range;
				out << YAML::Key << "Angle" << YAML::Value << lightComponent.Angle;
				out << YAML::Key << "AngleAttenuation" << YAML::Value << lightComponent.AngleAttenuation;
				out << YAML::Key << "Falloff" << YAML::Value << lightComponent.Falloff;
			});

		SerializeComponent<ScriptComponent>("ScriptComponent", entity, out, [&]()
			{
				auto& scriptComponent = entity.GetComponent<ScriptComponent>();
				out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;

				// Fields
				Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(scriptComponent.ClassName);
				const auto& fields = entityClass->GetFields();
				if (fields.size() > 0)
				{
					out << YAML::Key << "ScriptFields" << YAML::Value;
					auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
					out << YAML::BeginSeq;
					for (const auto& [name, field] : fields)
					{
						if (entityFields.find(name) == entityFields.end())
							continue;

						out << YAML::BeginMap; // ScriptField
						out << YAML::Key << "Name" << YAML::Value << name;
						out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);

						out << YAML::Key << "Data" << YAML::Value;
						ScriptFieldInstance& scriptField = entityFields.at(name);

						switch (field.Type)
						{
							WRITE_SCRIPT_FIELD(Float, float);
							WRITE_SCRIPT_FIELD(Double, double);
							WRITE_SCRIPT_FIELD(Bool, bool);
							WRITE_SCRIPT_FIELD(Char, char);
							WRITE_SCRIPT_FIELD(Byte, int8_t);
							WRITE_SCRIPT_FIELD(Short, int16_t);
							WRITE_SCRIPT_FIELD(Int, int32_t);
							WRITE_SCRIPT_FIELD(Long, int64_t);
							WRITE_SCRIPT_FIELD(UByte, uint8_t);
							WRITE_SCRIPT_FIELD(UShort, uint16_t);
							WRITE_SCRIPT_FIELD(UInt, uint32_t);
							WRITE_SCRIPT_FIELD(ULong, uint64_t);
							WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
							WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
							WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
							WRITE_SCRIPT_FIELD(Entity, UUID);
						}
						out << YAML::EndMap; // ScriptFields
					}
					out << YAML::EndSeq;
				}
			});

		SerializeComponent<SpriteRendererComponent>("SpriteRendererComponent", entity, out, [&]()
			{
				auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
				out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
				if (spriteRendererComponent.Texture)
					out << YAML::Key << "TexturePath" << YAML::Value << spriteRendererComponent.Texture->GetPath();

				out << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent.TilingFactor;
				out << YAML::Key << "UVStart" << YAML::Value << spriteRendererComponent.UVStart;
				out << YAML::Key << "UVEnd" << YAML::Value << spriteRendererComponent.UVEnd;
			});

		SerializeComponent<Rigidbody2DComponent>("Rigidbody2DComponent", entity, out, [&]()
			{
				auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
				out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
				out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;
				out << YAML::Key << "GravityScale" << YAML::Value << rb2dComponent.GravityScale;
			});

		SerializeComponent<BoxCollider2DComponent>("BoxCollider2DComponent", entity, out, [&]()
			{
				auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
				out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
				out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;
				out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
				out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
				out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
				out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;
			});

		SerializeComponent<CircleRendererComponent>("CircleRendererComponent", entity, out, [&]()
			{
				auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
				out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
				out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
				out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;
			});

		SerializeComponent<CircleCollider2DComponent>("CircleCollider2DComponent", entity, out, [&]()
			{
				auto& cc2dComponent = entity.GetComponent<CircleCollider2DComponent>();
				out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.Offset;
				out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.Radius;
				out << YAML::Key << "Density" << YAML::Value << cc2dComponent.Density;
				out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.Friction;
				out << YAML::Key << "Restitution" << YAML::Value << cc2dComponent.Restitution;
				out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2dComponent.RestitutionThreshold;
			});

		SerializeComponent<TextComponent>("TextComponent", entity, out, [&]()
			{
				auto& textComponent = entity.GetComponent<TextComponent>();
				out << YAML::Key << "TextString" << YAML::Value << textComponent.TextString;
				// TODO: textComponent.FontAsset
				out << YAML::Key << "Color" << YAML::Value << textComponent.Color;
				out << YAML::Key << "Kerning" << YAML::Value << textComponent.Kerning;
				out << YAML::Key << "LineSpacing" << YAML::Value << textComponent.LineSpacing;
			});

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		// Not implemented
		HNB_CORE_ASSERT(false);
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		} catch (YAML::ParserException e)
		{
			HNB_CORE_ERROR("Failed to load .Hanabi file '{0}'\n     {1}", filepath, e.what());
			return false;
		}
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		HNB_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				HNB_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					auto& mc = deserializedEntity.AddComponent<MeshComponent>();

					//TODO: this should handle by asset manager
					if (meshComponent["MeshPath"])
					{
						std::string meshPath = meshComponent["MeshPath"].as<std::string>();
						Ref<MeshSource> meshSource = CreateRef<MeshSource>(meshPath);
						mc.Mesh = CreateRef<Mesh>(meshSource);
					}
				}

				auto materialComponent = entity["MaterialComponent"];
				if (materialComponent)
				{
					auto& mtc = deserializedEntity.AddComponent<MaterialComponent>();
					mtc.Material = CreateRef<MaterialAsset>();
					//TODO: this should handle by asset manager
					if (materialComponent["DiffuseTexturePath"])
					{
						std::string diffusePath = materialComponent["DiffuseTexturePath"].as<std::string>();
						mtc.Material->SetDiffuse(Texture2D::Create(diffusePath));
					}

					if (materialComponent["SpecularTexturePath"])
					{
						std::string specularPath = materialComponent["SpecularTexturePath"].as<std::string>();
						mtc.Material->SetSpecular(Texture2D::Create(specularPath));
					}

					if (materialComponent["NormalTexturePath"])
					{
						std::string normalPath = materialComponent["NormalTexturePath"].as<std::string>();
						mtc.Material->SetNormal(Texture2D::Create(normalPath));
					}

					if (materialComponent["UseNormalMap"])
					{
						mtc.UseNormalMap = materialComponent["UseNormalMap"].as<bool>();
					}
				}

				auto lightComponent = entity["LightComponent"];
				if (lightComponent)
				{
					auto& lc = deserializedEntity.AddComponent<LightComponent>();
					lc.Type = LightTypeFromString(lightComponent["LightType"].as<std::string>());
					lc.Intensity = lightComponent["Intensity"].as<float>();
					lc.Radiance = lightComponent["Radiance"].as<glm::vec3>();
					lc.Radius = lightComponent["Radius"].as<float>();
					lc.Range = lightComponent["Range"].as<float>();
					lc.Angle = lightComponent["Angle"].as<float>();
					lc.AngleAttenuation = lightComponent["AngleAttenuation"].as<float>();
					lc.Falloff = lightComponent["Falloff"].as<float>();
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
					sc.ClassName = scriptComponent["ClassName"].as<std::string>();

					auto scriptFields = scriptComponent["ScriptFields"];
					if (scriptFields)
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
						if (entityClass)
						{
							const auto& fields = entityClass->GetFields();
							auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);
							for (auto scriptField : scriptFields)
							{
								std::string name = scriptField["Name"].as<std::string>();
								std::string typeString = scriptField["Type"].as<std::string>();
								ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

								ScriptFieldInstance& fieldInstance = entityFields[name];

								HNB_CORE_ASSERT(fields.find(name) != fields.end());

								if (fields.find(name) == fields.end())
									continue;

								fieldInstance.Field = fields.at(name);

								switch (type)
								{
									READ_SCRIPT_FIELD(Float, float);
									READ_SCRIPT_FIELD(Double, double);
									READ_SCRIPT_FIELD(Bool, bool);
									READ_SCRIPT_FIELD(Char, char);
									READ_SCRIPT_FIELD(Byte, int8_t);
									READ_SCRIPT_FIELD(Short, int16_t);
									READ_SCRIPT_FIELD(Int, int32_t);
									READ_SCRIPT_FIELD(Long, int64_t);
									READ_SCRIPT_FIELD(UByte, uint8_t);
									READ_SCRIPT_FIELD(UShort, uint16_t);
									READ_SCRIPT_FIELD(UInt, uint32_t);
									READ_SCRIPT_FIELD(ULong, uint64_t);
									READ_SCRIPT_FIELD(Vector2, glm::vec2);
									READ_SCRIPT_FIELD(Vector3, glm::vec3);
									READ_SCRIPT_FIELD(Vector4, glm::vec4);
									READ_SCRIPT_FIELD(Entity, UUID);
								}
							}
						}
					}
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					if (spriteRendererComponent["TexturePath"])
					{
						//TODO: move to asset manger
						std::string texturePath = spriteRendererComponent["TexturePath"].as<std::string>();
						src.Texture = Texture2D::Create(texturePath);
					}

					if (spriteRendererComponent["TilingFactor"])
						src.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
					if (spriteRendererComponent["UVStart"])
						src.UVStart = spriteRendererComponent["UVStart"].as<glm::vec2>();
					if (spriteRendererComponent["UVEnd"])
						src.UVEnd = spriteRendererComponent["UVEnd"].as<glm::vec2>();
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
					crc.Color = circleRendererComponent["Color"].as<glm::vec4>();
					crc.Thickness = circleRendererComponent["Thickness"].as<float>();
					crc.Fade = circleRendererComponent["Fade"].as<float>();
				}

				auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidbody2DComponent)
				{
					auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb2d.Type = RigidBody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
					rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
					rb2d.GravityScale = rigidbody2DComponent["GravityScale"].as<float>();
				}

				auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					bc2d.Density = boxCollider2DComponent["Density"].as<float>();
					bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
					bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
				if (circleCollider2DComponent)
				{
					auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
					cc2d.Density = circleCollider2DComponent["Density"].as<float>();
					cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
					cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				auto textComponent = entity["TextComponent"];
				if (textComponent)
				{
					auto& tc = deserializedEntity.AddComponent<TextComponent>();
					tc.TextString = textComponent["TextString"].as<std::string>();
					// TODO: textComponent.FontAsset
					tc.Color = textComponent["Color"].as<glm::vec4>();
					tc.Kerning = textComponent["Kerning"].as<float>();
					tc.LineSpacing = textComponent["LineSpacing"].as<float>();
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		// Not implemented
		HNB_CORE_ASSERT(false);
		return false;
	}
}