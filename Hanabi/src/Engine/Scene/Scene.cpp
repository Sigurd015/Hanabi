#include "hnbpch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Physics/Physics2D.h"
#include "Engine/Renderer/SceneRenderer.h"

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
		return newEntity;
	}

	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		bool IsExist = m_EntityMap.find(uuid) != m_EntityMap.end();
		HNB_CORE_ASSERT(IsExist);
		return { m_EntityMap.at(uuid), this };
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
		if (entity)
			return entity.GetComponent<TransformComponent>();
		else
			return {};
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMap[uuid] = entity;

		return entity;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	void Scene::DestroyEntity(Entity entity)
	{
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
				auto view = m_Registry.view<Rigidbody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

					b2Body* body = (b2Body*)rb2d.RuntimeBody;
					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
				}
			}
		}

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		m_Environment->PointLights.clear();
		m_Environment->SpotLights.clear();
		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if (camera.Primary)
			{
				mainCamera = &camera.Camera;
				cameraTransform = transform.GetTransform();
				m_Environment->CameraPosition = transform.Translation;
				break;
			}
		}

		if (mainCamera)
		{
			glm::mat4 viewProjection = mainCamera->GetProjection() * glm::inverse(cameraTransform);
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
					auto [transform, light] = view.get<TransformComponent, LightComponent>(entity);
					switch (light.Type)
					{
					case LightComponent::LightType::Directional:
					{
						m_Environment->DirLight = {
							light.Radiance,
							light.Intensity,
							-glm::normalize(glm::rotate(transform.GetRotation(), glm::vec3(0.0f,0.0f,-1.0f))),
							light.Shadow,
						};
						break;
					}
					case LightComponent::LightType::Point:
					{
						m_Environment->PointLights.push_back({
							transform.Translation,
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
							transform.Translation,
							light.Intensity,
							light.Radiance,
							light.AngleAttenuation,
							glm::normalize(glm::rotate(transform.GetRotation(), glm::vec3(1.0f, 0.0f, 0.0f))),
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
				auto [transform, mesh, material] = view.get<TransformComponent, MeshComponent, MaterialComponent>(entity);
				SceneRenderer::SubmitStaticMesh(transform.GetTransform(), mesh, material.MaterialAssetHandle);
			}
		}

		// Draw objects with default material
		{
			auto view = m_Registry.view<TransformComponent, MeshComponent>();
			for (auto entity : view)
			{
				auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);
				SceneRenderer::SubmitStaticMesh(transform.GetTransform(), mesh);
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
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite);
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle);
			}
		}

		// Draw text
		{
			auto view = m_Registry.view<TransformComponent, TextComponent>();
			for (auto entity : view)
			{
				auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

				Renderer2D::DrawString(transform.GetTransform(), text);
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
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(bc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.05f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.Translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
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
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(cc2d.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.05f);

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
			const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
			Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
		}
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
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