#include "hnbpch.h"
#include "Hanabi/Scripting/ScriptGlue.h"
#include "Hanabi/Scripting/ScriptEngine.h"
#include "Hanabi/Core/UUID.h"
#include "Hanabi/Input/KeyCodes.h"
#include "Hanabi/Input/Input.h"
#include "Hanabi/Scene/Scene.h"
#include "Hanabi/Scene/Entity.h"

#include <box2d/b2_body.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/object.h>

namespace Hanabi
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

	#define HNB_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Hanabi.InternalCalls::" #Name, Name)

	namespace Utils
	{
		static std::string MonoStringToUTF8(MonoString* monoString)
		{
			if (monoString == nullptr || mono_string_length(monoString) == 0)
				return "";

			char* str = mono_string_to_utf8(monoString);
			std::string result(str);
			mono_free(str);
			return result;
		}
	}

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

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3* outRotation)
	{
		Entity entity = GetEntity(entityID);

		*outRotation = entity.GetComponent<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3* rotation)
	{
		Entity entity = GetEntity(entityID);

		entity.GetComponent<TransformComponent>().Rotation = *rotation;
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

	static void SpriteRendererComponent_SetTextureCoords(UUID entityID, glm::vec2* start, glm::vec2* end)
	{
		Entity entity = GetEntity(entityID);
		auto& src = entity.GetComponent<SpriteRendererComponent>();
		src.UVStart = *start;
		src.UVEnd = *end;
	}

	static MonoString* TextComponent_GetText(UUID entityID)
	{
		Entity entity = GetEntity(entityID);
		auto& tc = entity.GetComponent<TextComponent>();
		return ScriptEngine::CreateString(tc.TextString.c_str());
	}

	static void TextComponent_SetText(UUID entityID, MonoString* textString)
	{
		Entity entity = GetEntity(entityID);
		auto& tc = entity.GetComponent<TextComponent>();
		tc.TextString = Utils::MonoStringToUTF8(textString);
	}

	static void TextComponent_GetColor(UUID entityID, glm::vec4* color)
	{
		Entity entity = GetEntity(entityID);
		auto& tc = entity.GetComponent<TextComponent>();
		*color = tc.Color;
	}

	static void TextComponent_SetColor(UUID entityID, glm::vec4* color)
	{
		Entity entity = GetEntity(entityID);
		auto& tc = entity.GetComponent<TextComponent>();
		tc.Color = *color;
	}

	static float TextComponent_GetKerning(UUID entityID)
	{
		Entity entity = GetEntity(entityID);
		auto& tc = entity.GetComponent<TextComponent>();
		return tc.Kerning;
	}

	static void TextComponent_SetKerning(UUID entityID, float kerning)
	{
		Entity entity = GetEntity(entityID);
		auto& tc = entity.GetComponent<TextComponent>();
		tc.Kerning = kerning;
	}

	static float TextComponent_GetLineSpacing(UUID entityID)
	{
		Entity entity = GetEntity(entityID);
		auto& tc = entity.GetComponent<TextComponent>();
		return tc.LineSpacing;
	}

	static void TextComponent_SetLineSpacing(UUID entityID, float lineSpacing)
	{
		Entity entity = GetEntity(entityID);
		auto& tc = entity.GetComponent<TextComponent>();
		tc.LineSpacing = lineSpacing;
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	static void Log_LogMessage(LogLevel level, MonoString* inFormattedMessage)
	{
		std::string message = Utils::MonoStringToUTF8(inFormattedMessage);
		switch (level)
		{
		case LogLevel::Trace:
			HNB_SCRIPT_TRACE(message);
			break;
		case LogLevel::Info:
			HNB_SCRIPT_INFO(message);
			break;
		case LogLevel::Warn:
			HNB_SCRIPT_WARN(message);
			break;
		case LogLevel::Error:
			HNB_SCRIPT_ERROR(message);
			break;
		case LogLevel::Critical:
			HNB_SCRIPT_CRITICAL(message);
			break;
		}
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
		HNB_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		HNB_ADD_INTERNAL_CALL(TransformComponent_SetRotation);

		HNB_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		HNB_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

		HNB_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTextureCoords);

		HNB_ADD_INTERNAL_CALL(TextComponent_GetText);
		HNB_ADD_INTERNAL_CALL(TextComponent_SetText);
		HNB_ADD_INTERNAL_CALL(TextComponent_GetColor);
		HNB_ADD_INTERNAL_CALL(TextComponent_SetColor);
		HNB_ADD_INTERNAL_CALL(TextComponent_GetKerning);
		HNB_ADD_INTERNAL_CALL(TextComponent_SetKerning);
		HNB_ADD_INTERNAL_CALL(TextComponent_GetLineSpacing);
		HNB_ADD_INTERNAL_CALL(TextComponent_SetLineSpacing);


		HNB_ADD_INTERNAL_CALL(Input_IsKeyDown);
		HNB_ADD_INTERNAL_CALL(Log_LogMessage);
	}
}