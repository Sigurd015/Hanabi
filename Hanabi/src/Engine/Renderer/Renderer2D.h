#pragma once
#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/UI/Font.h"
#include "Engine/Scene/Components.h"

namespace Hanabi
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();
		static void Flush();
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, glm::vec2 uv0, glm::vec2 uv1,
			const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f, int entityID = -1);
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color,
			float thickness = 1.0f, float fade = 0.005f, int entityID = -1);
		static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static float GetLineWidth();
		static void SetLineWidth(float width);

		struct TextParams
		{
			glm::vec4 Color{ 1.0f };
			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};
		static void DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID = -1);
		static void DrawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entityID = -1);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};
		static void ResetStats();
		static Statistics GetStats();
	private:
		static float GetTextureID(const Ref<Texture2D>& texture);
		static void SetQuadVertex(const glm::mat4& transform, const glm::vec4& color,
			int entityID, const glm::vec2* texCoord, float texIndex, float tilingFactor);
		static void StartBatch();
		static void NextBatch();
	};
}