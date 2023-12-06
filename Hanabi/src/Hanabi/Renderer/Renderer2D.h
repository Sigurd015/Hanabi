#pragma once
#include "Camera.h"
#include "Texture.h"
#include "UI/Font.h"
#include "Hanabi/Scene/Components.h"
#include "RenderPass.h"

namespace Hanabi
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const glm::mat4& viewProjection);
		static void EndScene();

		static void SetTargetFramebuffer(const Ref<Framebuffer>& framebuffer);
		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src);
		static void DrawCircle(const glm::mat4& transform, CircleRendererComponent& crc);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, glm::vec2 uv0, glm::vec2 uv1,
			const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f);
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color,
			float thickness = 1.0f, float fade = 0.005f);
		static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color);
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color);
		static float GetLineWidth();
		static void SetLineWidth(float width);

		struct TextParams
		{
			glm::vec4 Color{ 1.0f };
			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};
		static void DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams);
		static void DrawString(const glm::mat4& transform, const TextComponent& tc);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t LineCount = 0;

			uint32_t GetTotalVertexCount() const { return QuadCount * 4 + LineCount * 2; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6 + LineCount * 2; }
		};

		static void ResetStats();
		static Statistics GetStats();
	private:
		static uint32_t GetTextureID(const Ref<Texture2D>& texture);
		static void SetQuadVertex(const glm::mat4& transform, const glm::vec4& color,
			const glm::vec2* texCoord, uint32_t texIndex, float tilingFactor);
		static void StartBatch();
		static void Flush();
		static void NextBatch();
	};
}