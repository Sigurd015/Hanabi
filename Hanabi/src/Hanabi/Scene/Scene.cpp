#include "hnbpch.h"
#include "Hanabi/Scene/Scene.h"
#include "Hanabi/Renderer/Renderer.h"
#include "Hanabi/Scene/Components.h"
#include "Hanabi/Scene/Entity.h"
#include "Hanabi/Scripting/ScriptEngine.h"
#include "Hanabi/Physics/Physics2D.h"
#include "Hanabi/Renderer/SceneRenderer.h"

// Box2D
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

namespace Hanabi
{
	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src,
		const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
			{
				auto view = src.view<Component>();
				for (auto srcEntity : view)
				{
					entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);
					auto& srcComponent = src.get<Component>(srcEntity);
					dst.emplace_or_replace<Component>(dstEntity, srcComponent);
				}
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst,
		entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
			{
				if (src.HasComponent<Component>())
					dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
			}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	Scene::Scene() :m_Environment(CreateRef<Environment>())
	{}

	Scene::~Scene()
	{
		delete m_PhysicsWorld;
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components (except IDComponent and TagComponent)
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);
		return newScene;
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);

		auto parentEntity = entity.GetParentUUID();
		if (parentEntity)
		{
			newEntity.SetParentUUID(parentEntity);
			auto parent = GetEntityByUUID(parentEntity);
			parent.GetChildren().push_back(newEntity.GetUUID());
		}

		return newEntity;
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		bool IsExist = m_EntityMap.find(uuid) != m_EntityMap.end();
		HNB_CORE_ASSERT(IsExist);
		return { m_EntityMap.at(uuid), this };
	}

	Entity Scene::TryGetEntityByUUID(UUID uuid)
	{
		if (auto iter = m_EntityMap.find(uuid); iter != m_EntityMap.end())
			return { iter->second, this };
		return Entity{};
	}

	Entity Scene::FindEntityByName(std::string_view name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return Entity{ entity, this };
		}
		return {};
	}

	TransformComponent Scene::GetWorldSpaceTransform(Entity entity)
	{
		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		TransformComponent transformComponent;
		transformComponent.SetTransform(transform);
		return transformComponent;
	}

	glm::mat4 Scene::GetWorldSpaceTransformMatrix(Entity entity)
	{
		glm::mat4 transform(1.0f);

		Entity parent = TryGetEntityByUUID(entity.GetParentUUID());
		if (parent)
			transform = GetWorldSpaceTransformMatrix(parent);

		return transform * entity.Transform().GetTransform();
	}

	Entity Scene::GetMainCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cc = view.get<CameraComponent>(entity);
			if (cc.Primary)
			{
				HNB_CORE_ASSERT(cc.Camera.GetOrthographicSize() || cc.Camera.GetPerspectiveVerticalFOV(), "Camera is not fully initialized");
				return { entity, this };
			}
		}
		return {};
	}

	void Scene::ParentEntity(Entity entity, Entity parent)
	{
		if (parent.IsDescendantOf(entity))
		{
			UnparentEntity(parent);

			Entity newParent = TryGetEntityByUUID(entity.GetParentUUID());
			if (newParent)
			{
				ParentEntity(parent, newParent);
			}
		}
		else
		{
			Entity previousParent = TryGetEntityByUUID(entity.GetParentUUID());

			if (previousParent)
				UnparentEntity(entity);
		}

		entity.SetParentUUID(parent.GetUUID());
		parent.GetChildren().push_back(entity.GetUUID());

		ConvertToLocalSpace(entity);
	}

	void Scene::UnparentEntity(Entity entity, bool convertToWorldSpace)
	{
		Entity parent = GetEntityByUUID(entity.GetParentUUID());
		if (!parent)
			return;

		auto& parentChildren = parent.GetChildren();
		parentChildren.erase(std::remove(parentChildren.begin(), parentChildren.end(), entity.GetUUID()), parentChildren.end());

		if (convertToWorldSpace)
			ConvertToWorldSpace(entity);

		entity.SetParentUUID(0);
	}

	void Scene::ConvertToLocalSpace(Entity entity)
	{
		Entity parent = TryGetEntityByUUID(entity.GetParentUUID());

		if (!parent)
			return;

		auto& transform = entity.Transform();
		glm::mat4 parentTransform = GetWorldSpaceTransformMatrix(parent);
		glm::mat4 localTransform = glm::inverse(parentTransform) * transform.GetTransform();
		transform.SetTransform(localTransform);
	}

	void Scene::ConvertToWorldSpace(Entity entity)
	{
		Entity parent = TryGetEntityByUUID(entity.GetParentUUID());

		if (!parent)
			return;

		glm::mat4 transform = GetWorldSpaceTransformMatrix(entity);
		auto& entityTransform = entity.Transform();
		entityTransform.SetTransform(transform);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name.empty() ? Entity::NoName : name);
		entity.AddComponent<RelationshipComponent>();

		m_EntityMap[uuid] = entity;

		return entity;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	void Scene::DestroyEntity(Entity entity)
	{
		// TODO: Fix crash when destroying entity with children
		//// If entity has children, destroy them first
		//auto& children = entity.GetChildren();
		//for (size_t i = 0; i < children.size(); i++)
		//{
		//	auto childId = children[i];
		//	DestroyEntity(GetEntityByUUID(childId));
		//}

		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		OnPhysics2DStart();

		// Scripting
		{
			ScriptEngine::OnRuntimeStart(this);
			// Instantiate all script entities

			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::OnCreateEntity(entity);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

		OnPhysics2DStop();
		ScriptEngine::OnRuntimeStop();
	}

	void Scene::Step(int frames)
	{
		m_StepFrames = frames;
	}

	void Scene::OnUpdateRuntime(Timestep ts, Entity selectedEntity, bool enableOverlayRender)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Update scripts
			{
				auto view = m_Registry.view<ScriptComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					ScriptEngine::OnUpdateEntity(entity, ts);
				}
			}

			// Update Physics
			{
				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;
				m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);
				// Retrieve transform from Box2D
				auto view = m_Registry.view<TransformComponent, Rigidbody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = GetWorldSpaceTransform(entity);
					auto [tc, rb2d] = view.get<TransformComponent, Rigidbody2DComponent>(entity);

					b2Body* body = (b2Body*)rb2d.RuntimeBody;
					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
					tc.SetTransform(transform.GetTransform());
				}
			}
		}

		// Find main camera
		Entity mainCameraEntity = GetMainCameraEntity();

		if (mainCameraEntity)
		{
			glm::mat4 cameraTransform = glm::inverse(GetWorldSpaceTransformMatrix(mainCameraEntity));
			m_Environment->PointLights.clear();
			m_Environment->SpotLights.clear();
			auto worldTransform = GetWorldSpaceTransform({ mainCameraEntity, this });
			m_Environment->CameraPosition = worldTransform.Translation;

			glm::mat4 viewProjection = mainCameraEntity.GetComponent<CameraComponent>().Camera.GetProjection() * cameraTransform;
			m_Environment->ViewProjection = viewProjection;
			RenderScene(selectedEntity, enableOverlayRender);
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera, Entity selectedEntity, bool enableOverlayRender)
	{
		m_Environment->PointLights.clear();
		m_Environment->SpotLights.clear();
		m_Environment->CameraPosition = camera.GetPosition();
		m_Environment->ViewProjection = camera.GetViewProjection();
		RenderScene(selectedEntity, enableOverlayRender);
	}

	void Scene::RenderScene(Entity selectedEntity, bool enableOverlayRender)
	{
		//----------------- 3D Scene Rendering -----------------//				
		// Lights
		{
			{
				auto view = m_Registry.view<SkyLightComponent>();
				if (view.empty())
				{
					m_Environment->EnvMapHandle = 0;
					m_Environment->SkyLightIntensity = 0.0f;
				}
				else
				{
					// Only one skylight is allowed
					auto& skylight = view.get<SkyLightComponent>(view.front());
					m_Environment->EnvMapHandle = skylight.EnvMapHandle;
					m_Environment->SkyLightIntensity = skylight.Intensity;
				}
			}

			{
				auto view = m_Registry.view<TransformComponent, LightComponent>();
				for (auto entity : view)
				{
					auto light = view.get<LightComponent>(entity);
					auto worldTransform = GetWorldSpaceTransform({ entity, this });
					switch (light.Type)
					{
					case LightComponent::LightType::Directional:
					{
						m_Environment->DirLight = {
							light.Radiance,
							light.Intensity,
							-glm::normalize(glm::mat3(worldTransform.GetTransform()) * glm::vec3(1.0f)),
							//-glm::normalize(glm::rotate(transform.GetRotation(), glm::vec3(0.0f,0.0f,-1.0f))),
							light.Shadow,
						};
						break;
					}
					case LightComponent::LightType::Point:
					{
						m_Environment->PointLights.push_back({
							worldTransform.Translation,
							light.Intensity,
							light.Radiance,
							light.Radius,
							light.Falloff,
							light.Shadow,
							});
						break;
					}
					case LightComponent::LightType::Spot:
					{
						m_Environment->SpotLights.push_back({
							worldTransform.Translation,
							light.Intensity,
							light.Radiance,
							light.AngleAttenuation,
							glm::normalize(glm::rotate(worldTransform.GetRotation(), glm::vec3(1.0f, 0.0f, 0.0f))),
							light.Range,
							light.Angle,
							light.Falloff,
							light.Shadow,
							});
						break;
					}
					}
				}
			}
		}

		SceneRenderer::BeginScene(m_Environment);

		// Draw objects with materials
		{
			auto view = m_Registry.view<TransformComponent, MeshComponent, MaterialComponent>();
			for (auto entity : view)
			{
				auto [mesh, material] = view.get<MeshComponent, MaterialComponent>(entity);
				auto worldTransform = GetWorldSpaceTransformMatrix({ entity, this });

				if (mesh.SubmeshIndex)
					SceneRenderer::SubmitStaticMesh(worldTransform, mesh, material.MaterialAssetHandle, mesh.SubmeshIndex);
				else
					SceneRenderer::SubmitStaticMesh(worldTransform, mesh, material.MaterialAssetHandle);
			}
		}

		// Draw objects with default material
		{
			auto view = m_Registry.view<TransformComponent, MeshComponent>();
			for (auto entity : view)
			{
				auto mesh = view.get<MeshComponent>(entity);
				auto worldTransform = GetWorldSpaceTransformMatrix({ entity, this });
				SceneRenderer::SubmitStaticMesh(worldTransform, mesh);
			}
		}

		SceneRenderer::EndScene();

		//----------------- 2D Scene Rendering -----------------//
		Renderer2D::BeginScene(m_Environment->ViewProjection);
		Renderer2D::SetTargetFramebuffer(SceneRenderer::GetFinalPass()->GetTargetFramebuffer());
		// Draw sprites
		{
			auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto entity : view)
			{
				auto sprite = view.get<SpriteRendererComponent>(entity);
				auto worldTransform = GetWorldSpaceTransformMatrix({ entity, this });
				Renderer2D::DrawSprite(worldTransform, sprite);
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto circle = view.get<CircleRendererComponent>(entity);
				auto worldTransform = GetWorldSpaceTransformMatrix({ entity, this });
				Renderer2D::DrawCircle(worldTransform, circle);
			}
		}

		// Draw text
		{
			auto view = m_Registry.view<TransformComponent, TextComponent>();
			for (auto entity : view)
			{
				auto text = view.get<TextComponent>(entity);
				auto worldTransform = GetWorldSpaceTransformMatrix({ entity, this });
				Renderer2D::DrawString(worldTransform, text);
			}
		}

		OnOverlayRender(enableOverlayRender, selectedEntity);

		Renderer2D::EndScene();
	}

	//TODO: Make a better way to render overlays
	void Scene::OnOverlayRender(bool enable, Entity selectedEntity)
	{
		if (enable)
		{
			// Box Colliders
			{
				auto view = GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto bc2d = view.get<BoxCollider2DComponent>(entity);
					auto worldTransform = GetWorldSpaceTransform({ entity, this });
					glm::vec3 translation = worldTransform.Translation + glm::vec3(bc2d.Offset, 0.001f);
					glm::vec3 scale = worldTransform.Scale * glm::vec3(bc2d.Size * 2.05f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), worldTransform.Translation)
						* glm::rotate(glm::mat4(1.0f), worldTransform.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.Offset, 0.001f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			// Circle Colliders
			{
				auto view = GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto cc2d = view.get<CircleCollider2DComponent>(entity);
					auto worldTransform = GetWorldSpaceTransform({ entity, this });
					glm::vec3 translation = worldTransform.Translation + glm::vec3(cc2d.Offset, 0.001f);
					glm::vec3 scale = worldTransform.Scale * glm::vec3(cc2d.Radius * 2.05f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
				}
			}
		}

		// Draw selected entity outline 
		// TODO: Make this work for 3D Objects
		if (selectedEntity)
		{
			auto worldTransform = GetWorldSpaceTransformMatrix({ selectedEntity, this });
			Renderer2D::DrawRect(worldTransform, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
		}
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = GetWorldSpaceTransform(entity);
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = bodyDef.type = Utils::Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			body->SetGravityScale(rb2d.GravityScale);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = transform.Scale.x * cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;

		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		//static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}
}