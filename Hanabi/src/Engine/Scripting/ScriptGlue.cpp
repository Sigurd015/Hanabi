#include "hnbpch.h"
#include "Engine/Scripting/ScriptGlue.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Core/UUID.h"
#include "Engine/Input/KeyCodes.h"
#include "Engine/Input/Input.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

#include <box2d/b2_body.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/object.h>

namespace Hanabi
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define HNB_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Hanabi.InternalCalls::" #Name, Name)

	static Entity GetEntity(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HNB_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HNB_CORE_ASSERT(entity);
		return  entity;
	}

	static MonoObject* GetScriptInstance(UUID entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}

	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::GetSceneContext();
		HNB_CORE_ASSERT(scene);
		Entity entity = scene->FindEntityByName(nameCStr);
		mono_free(nameCStr);

		if (!entity)
			return 0;

		return entity.GetUUID();
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Entity entity = GetEntity(entityID);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		HNB_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Entity entity = GetEntity(entityID);

		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Entity entity = GetEntity(entityID);

		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Entity entity = GetEntity(entityID);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Entity entity = GetEntity(entityID);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("Hanabi.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					HNB_CORE_ERROR("[ScriptEngine] Could not find component type {}", managedTypename);
					return;
				}
				s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		HNB_ADD_INTERNAL_CALL(GetScriptInstance);
		HNB_ADD_INTERNAL_CALL(Entity_HasComponent);
		HNB_ADD_INTERNAL_CALL(Entity_FindEntityByName);

		HNB_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		HNB_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		HNB_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		HNB_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

		HNB_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
}