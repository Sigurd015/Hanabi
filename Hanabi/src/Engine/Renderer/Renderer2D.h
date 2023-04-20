#pragma once
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Texture.h"

namespace Hanabi
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const Camera& camera);
		static void EndScene();
		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const float angle, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const float angle, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const float angle, const Ref<Texture2D>& texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const float angle, const Ref<Texture2D>& texture);
	};
}