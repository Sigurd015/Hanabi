#include "hnbpch.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Physics/Physics2D.h"

namespace Hanabi
{
	static glm::vec4 s_EditorClearColor = { 0.3f, 0.3f, 0.3f, 1.0f };

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

	Scene::Scene()
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
				const int32_t iterations = 10;
				m_PhysicsWorld->Step(ts, iterations);

				// Retrieve transform from Box2D
				auto view = m_Registry.view<Rigidbody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

					Physix2D::Rigidbody2D* body = (Physix2D::Rigidbody2D*)rb2d.RuntimeBody;
					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
				}
			}
		}

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		Environment sceneEnvironment;
		auto view = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if (camera.Primary)
			{
				mainCamera = &camera.Camera;
				cameraTransform = transform.GetTransform();
				sceneEnvironment.CameraPosition = transform.Translation;
				sceneEnvironment.ClearType = camera.ClearType;
				sceneEnvironment.ClearColor = camera.ClearColor;
				sceneEnvironment.SkyboxAssetHandle = camera.SkyboxHandle;
				break;
			}
		}

		if (mainCamera)
		{
			glm::mat4 viewProjection = mainCamera->GetProjection() * glm::inverse(cameraTransform);
			sceneEnvironment.ViewProjection = viewProjection;
			RenderScene(sceneEnvironment, selectedEntity, enableOverlayRender);
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera, Entity selectedEntity, bool enableOverlayRender)
	{
		Environment sceneEnvironment = { camera.GetPosition() , camera.GetViewProjection() };
		sceneEnvironment.ClearType = CameraComponent::ClearMethod::Soild_Color;
		sceneEnvironment.ClearColor = s_EditorClearColor;
		RenderScene(sceneEnvironment, selectedEntity, enableOverlayRender);
	}

	void Scene::RenderScene(Environment& sceneEnvironment, Entity selectedEntity, bool enableOverlayRender)
	{
		//----------------- 3D Scene Rendering -----------------//				
		// Lights
		{
			auto view = m_Registry.view<TransformComponent, LightComponent>();
			for (auto entity : view)
			{
				auto [transform, light] = view.get<TransformComponent, LightComponent>(entity);
				switch (light.Type)
				{
				case LightComponent::LightType::Directional:
				{
					sceneEnvironment.DirLight = {
						light.Radiance,
						light.Intensity,
						-glm::normalize(glm::mat3(transform.GetTransform()) * glm::vec3(1.0f)),
					};
					break;
				}
				case LightComponent::LightType::Point:
				{
					sceneEnvironment.PointLights.push_back({
						transform.Translation,
						light.Intensity,
						light.Radiance,
						light.Radius,
						light.Falloff,
						});
					break;
				}
				case LightComponent::LightType::Spot:
				{
					sceneEnvironment.SpotLights.push_back({
						transform.Translation,
						light.Intensity,
						light.Radiance,
						light.AngleAttenuation,
						glm::normalize(glm::rotate(transform.GetRotation(), glm::vec3(1.0f, 0.0f, 0.0f))),
						light.Range,
						light.Angle,
						light.Falloff,
						});
					break;
				}
				}
			}
		}

		SceneRenderer::BeginScene(sceneEnvironment);

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
		Renderer2D::BeginScene(sceneEnvironment.ViewProjection);
		Renderer2D::SetTargetRenderPass(SceneRenderer::GetFinalRenderPass());
		// Draw sprites
		{
			auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}

		// Draw circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle, (int)entity);
			}
		}

		// Draw text
		{
			auto view = m_Registry.view<TransformComponent, TextComponent>();
			for (auto entity : view)
			{
				auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

				Renderer2D::DrawString(transform.GetTransform(), text, (int)entity);
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
		Physix2D::PhysicsWorld2DSpecification spec;
		spec.Gravity = { 0.0f, -9.8f };
		m_PhysicsWorld = new Physix2D::PhysicsWorld2D(spec);

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			Physix2D::Rigidbody2DSpecification rb2dSpec;
			rb2dSpec.Type = Utils::Rigidbody2DTypeToPhysix2DBody(rb2d.Type);
			rb2dSpec.Position = { transform.Translation.x, transform.Translation.y };
			rb2dSpec.Angle = transform.Rotation.z;
			rb2dSpec.FixedRotation = rb2d.FixedRotation;
			rb2dSpec.GravityScale = rb2d.GravityScale;

			Physix2D::Rigidbody2D* body = m_PhysicsWorld->CreateBody(rb2dSpec);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				Physix2D::BoxCollider boxCollider;
				boxCollider.Size = { bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y };
				boxCollider.Offset = { bc2d.Offset.x, bc2d.Offset.y };

				Physix2D::ShapeSpecification shapeSpec;
				shapeSpec.Collider = &boxCollider;
				shapeSpec.Density = bc2d.Density;
				shapeSpec.Friction = bc2d.Friction;
				shapeSpec.Restitution = bc2d.Restitution;
				shapeSpec.RestitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateShape(shapeSpec);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				Physix2D::CircleCollider circleCollider;
				circleCollider.Radius = cc2d.Radius * transform.Scale.x;
				circleCollider.Offset = { cc2d.Offset.x, cc2d.Offset.y };

				Physix2D::ShapeSpecification shapeSpec;
				shapeSpec.Collider = &circleCollider;
				shapeSpec.Density = cc2d.Density;
				shapeSpec.Friction = cc2d.Friction;
				shapeSpec.Restitution = cc2d.Restitution;
				shapeSpec.RestitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateShape(shapeSpec);
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