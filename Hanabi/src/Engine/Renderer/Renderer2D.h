#pragma once
#include "Camera.h"
#include "EditorCamera.h"
#include "Texture.h"
#include "UI/Font.h"
#include "Engine/Scene/Components.h"
#include "RenderPass.h"

namespace Hanabi
{
	class Renderer2D
	{
	public:
		void Init();
		void Shutdown();

		void Renderer2D::SetViewProjection(const glm::mat4& viewProjection);
		void Flush();

		Ref<RenderPass> GetTargetRenderPass();
		void SetTargetRenderPass(const Ref<RenderPass>& renderPass);

		void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, glm::vec2 uv0, glm::vec2 uv1,
			const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor = 1.0f, int entityID = -1);
		void DrawCircle(const glm::mat4& transform, const glm::vec4& color,
			float thickness = 1.0f, float fade = 0.005f, int entityID = -1);
		void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);
		void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		float GetLineWidth();
		void SetLineWidth(float width);

		struct TextParams
		{
			glm::vec4 Color{ 1.0f };
			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};
		void DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID = -1);
		void DrawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entityID = -1);

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
		float GetTextureID(const Ref<Texture2D>& texture);
		void SetQuadVertex(const glm::mat4& transform, const glm::vec4& color,
			int entityID, const glm::vec2* texCoord, float texIndex, float tilingFactor);
		void StartBatch();
		void NextBatch();

		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			int TexIndex;
			float TilingFactor;

			// Editor-only
			int EntityID;
		};

		struct CircleVertex
		{
			glm::vec3 WorldPosition;
			glm::vec3 LocalPosition;
			glm::vec4 Color;
			float Thickness;
			float Fade;

			// Editor-only
			int EntityID;
		};

		struct LineVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;

			// Editor-only
			int EntityID;
		};

		struct TextVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
			glm::vec2 TexCoord;

			// TODO: bg color for outline/bg

			// Editor-only
			int EntityID;
		};

		struct Renderer2DData
		{
			static const uint32_t MaxQuads = 10000;
			static const uint32_t MaxVertices = MaxQuads * 4;
			static const uint32_t MaxIndices = MaxQuads * 6;
			static const uint32_t MaxTextureSlots = 32;

			Ref<Pipeline> QuadPipeline;
			Ref<VertexBuffer> QuadVertexBuffer;
			Ref<IndexBuffer> QuadIndexBuffer;
			Ref<Material> QuadMaterial;
			uint32_t QuadIndexCount = 0;
			QuadVertex* QuadVertexBufferBase = nullptr;
			QuadVertex* QuadVertexBufferPtr = nullptr;

			Ref<Pipeline> CirclePipeline;
			Ref<VertexBuffer> CircleVertexBuffer;
			Ref<IndexBuffer> CircleIndexBuffer;
			Ref<Material> CircleMaterial;
			uint32_t CircleIndexCount = 0;
			CircleVertex* CircleVertexBufferBase = nullptr;
			CircleVertex* CircleVertexBufferPtr = nullptr;

			Ref<Pipeline> LinePipeline;
			Ref<VertexBuffer> LineVertexBuffer;
			Ref<Material> LineMaterial;
			uint32_t LineVertexCount = 0;
			LineVertex* LineVertexBufferBase = nullptr;
			LineVertex* LineVertexBufferPtr = nullptr;
			float LineWidth = 2.0f;

			Ref<Pipeline> TextPipeline;
			Ref<VertexBuffer> TextVertexBuffer;
			Ref<IndexBuffer> TextIndexBuffer;
			Ref<Material> TextMaterial;
			uint32_t TextIndexCount = 0;
			TextVertex* TextVertexBufferBase = nullptr;
			TextVertex* TextVertexBufferPtr = nullptr;

			std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;// 0 is white texture
			uint32_t TextureSlotIndex = 1;
			Ref<Texture2D> FontAtlasTexture;
			Ref<Texture2D> WhiteTexture;

			glm::vec4 QuadVertexPositions[4] =
			{ { -0.5f, -0.5f, 0.0f, 1.0f },
			  {  0.5f, -0.5f, 0.0f, 1.0f },
			  {  0.5f,  0.5f, 0.0f, 1.0f },
			  { -0.5f,  0.5f, 0.0f, 1.0f } };
			glm::vec2 QuadTexCoord[4] =
			{ { 0.0f, 0.0f },
			  { 1.0f, 0.0f },
			  { 1.0f, 1.0f },
			  { 0.0f, 1.0f } };

			struct CameraData
			{
				glm::mat4 ViewProjection;
			};
			CameraData CameraBuffer;
			Ref<ConstantBuffer> CameraConstantBuffer;
		};
		Renderer2DData m_Data;
	};
}