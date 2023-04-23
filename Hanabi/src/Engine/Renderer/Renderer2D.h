#pragma once
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/SubTexture2D.h"

namespace Hanabi
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const Camera& camera);
		static void EndScene();
		static void Flush();
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subTexture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};
		static void ResetStats();
		static Statistics GetStats();
	private:
		static void SetQuadVertex(const glm::mat4& transform, const glm::vec4& color, const glm::vec2* texCoord, float texIndex, float tilingFactor);
		static void FlushAndReset();
	};
}