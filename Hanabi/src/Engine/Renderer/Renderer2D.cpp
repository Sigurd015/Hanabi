#include "hnbpch.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Pipeline.h"
#include "Shader.h"
#include "Renderer.h"
#include "ConstantBuffer.h"
#include "UI/MSDFData.h"
#include "Engine/Asset/AssetManager/AssetManager.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hanabi
{
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
		CameraData SceneBuffer;
		Ref<ConstantBuffer> CameraConstantBuffer;
		Renderer2D::Statistics RendererStats;

		Ref<RenderPass> TatgetRenderPass;
	};
	static Renderer2DData* s_Data;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DData();
		//Quad
		{
			VertexBufferLayout layout = {
			   { ShaderDataType::Float3, "a_Position"     },
			   { ShaderDataType::Float4, "a_Color"        },
			   { ShaderDataType::Float2, "a_TexCoord"     },
			   { ShaderDataType::Int,    "a_TexIndex"     },
			   { ShaderDataType::Float,  "a_TilingFactor" },
			   { ShaderDataType::Int,    "a_EntityID"     }
			};
			s_Data->QuadVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(QuadVertex));
			s_Data->QuadVertexBuffer->SetLayout(layout);

			uint32_t* indices = new uint32_t[s_Data->MaxIndices];
			uint32_t offset = 0;
			for (uint32_t i = 0; i < s_Data->MaxIndices; i += 6)
			{
				indices[i + 0] = offset + 2;
				indices[i + 1] = offset + 1;
				indices[i + 2] = offset + 0;

				indices[i + 3] = offset + 0;
				indices[i + 4] = offset + 3;
				indices[i + 5] = offset + 2;

				offset += 4;
			}
			s_Data->QuadIndexBuffer = IndexBuffer::Create(indices, s_Data->MaxIndices);
			delete[] indices;

			PipelineSpecification pipelineSpec;
			pipelineSpec.Layout = layout;
			pipelineSpec.Shader = Renderer::GetShader("Renderer2D_Quad");
			pipelineSpec.Topology = PrimitiveTopology::Triangles;

			s_Data->QuadPipeline = Pipeline::Create(pipelineSpec);
			s_Data->QuadMaterial = Material::Create(pipelineSpec.Shader);

			s_Data->QuadVertexBufferBase = new QuadVertex[s_Data->MaxVertices];
		}

		// Circles
		{
			VertexBufferLayout layout = {
			  { ShaderDataType::Float3, "a_WorldPosition" },
			  { ShaderDataType::Float3, "a_LocalPosition" },
			  { ShaderDataType::Float4, "a_Color"         },
			  { ShaderDataType::Float,  "a_Thickness"     },
			  { ShaderDataType::Float,  "a_Fade"          },
			  { ShaderDataType::Int,    "a_EntityID"      }
			};
			s_Data->CircleVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(CircleVertex));
			s_Data->CircleVertexBuffer->SetLayout(layout);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Layout = layout;
			pipelineSpec.Shader = Renderer::GetShader("Renderer2D_Circle");
			pipelineSpec.Topology = PrimitiveTopology::Triangles;

			s_Data->CirclePipeline = Pipeline::Create(pipelineSpec);
			s_Data->CircleMaterial = Material::Create(pipelineSpec.Shader);

			s_Data->CircleVertexBufferBase = new CircleVertex[s_Data->MaxVertices];
		}

		// Lines
		{
			VertexBufferLayout layout = {
			  { ShaderDataType::Float3, "a_Position" },
			  { ShaderDataType::Float4, "a_Color"    },
			  { ShaderDataType::Int,    "a_EntityID" }
			};
			s_Data->LineVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(LineVertex));
			s_Data->LineVertexBuffer->SetLayout(layout);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Layout = layout;
			pipelineSpec.Shader = Renderer::GetShader("Renderer2D_Line");
			pipelineSpec.Topology = PrimitiveTopology::Lines;

			s_Data->LinePipeline = Pipeline::Create(pipelineSpec);
			s_Data->LineMaterial = Material::Create(pipelineSpec.Shader);

			s_Data->LineVertexBufferBase = new LineVertex[s_Data->MaxVertices];
		}

		// Text
		{
			VertexBufferLayout layout = {
			  { ShaderDataType::Float3, "a_Position"     },
			  { ShaderDataType::Float4, "a_Color"        },
			  { ShaderDataType::Float2, "a_TexCoord"     },
			  { ShaderDataType::Int,    "a_EntityID"     }
			};
			s_Data->TextVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(TextVertex));
			s_Data->TextVertexBuffer->SetLayout(layout);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Layout = layout;
			pipelineSpec.Shader = Renderer::GetShader("Renderer2D_Text");
			pipelineSpec.Topology = PrimitiveTopology::Triangles;

			s_Data->TextPipeline = Pipeline::Create(pipelineSpec);
			s_Data->TextMaterial = Material::Create(pipelineSpec.Shader);

			s_Data->TextVertexBufferBase = new TextVertex[s_Data->MaxVertices];
		}

		// Set WhiteTexture slots to 0
		s_Data->WhiteTexture = Renderer::GetTexture<Texture2D>("White");
		s_Data->TextureSlots[0] = s_Data->WhiteTexture;

		s_Data->CameraConstantBuffer = ConstantBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

		s_Data->QuadPipeline->SetConstantBuffer(s_Data->CameraConstantBuffer);
		s_Data->CirclePipeline->SetConstantBuffer(s_Data->CameraConstantBuffer);
		s_Data->LinePipeline->SetConstantBuffer(s_Data->CameraConstantBuffer);
		s_Data->TextPipeline->SetConstantBuffer(s_Data->CameraConstantBuffer);
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_Data->QuadVertexBufferBase;
		delete[] s_Data->CircleVertexBufferBase;
		delete[] s_Data->LineVertexBufferBase;
		delete[] s_Data->TextVertexBufferBase;

		delete s_Data;
	}

	void Renderer2D::BeginScene(const glm::mat4& viewProjection)
	{
		s_Data->SceneBuffer.ViewProjection = viewProjection;

		s_Data->CameraConstantBuffer->SetData(&s_Data->SceneBuffer, sizeof(Renderer2DData::CameraData));

		s_Data->QuadPipeline->SetConstantBuffer(s_Data->CameraConstantBuffer);
		s_Data->CirclePipeline->SetConstantBuffer(s_Data->CameraConstantBuffer);
		s_Data->LinePipeline->SetConstantBuffer(s_Data->CameraConstantBuffer);
		s_Data->TextPipeline->SetConstantBuffer(s_Data->CameraConstantBuffer);

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	Ref<RenderPass> Renderer2D::GetTargetRenderPass()
	{
		return s_Data->TatgetRenderPass;
	}

	void Renderer2D::SetTargetRenderPass(const Ref<RenderPass>& renderPass)
	{
		if (renderPass != s_Data->TatgetRenderPass)
		{
			s_Data->TatgetRenderPass = renderPass;
		}
	}

	void Renderer2D::StartBatch()
	{
		ResetStats();

		s_Data->QuadIndexCount = 0;
		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;

		s_Data->CircleIndexCount = 0;
		s_Data->CircleVertexBufferPtr = s_Data->CircleVertexBufferBase;

		s_Data->LineVertexCount = 0;
		s_Data->LineVertexBufferPtr = s_Data->LineVertexBufferBase;

		s_Data->TextIndexCount = 0;
		s_Data->TextVertexBufferPtr = s_Data->TextVertexBufferBase;

		s_Data->TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		Renderer::BeginRenderPass(s_Data->TatgetRenderPass, false);

		if (s_Data->QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase);
			s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
				s_Data->QuadMaterial->SetTexture(s_Data->TextureSlots[i], i);

			Renderer::DrawIndexed(s_Data->QuadVertexBuffer, s_Data->QuadIndexBuffer, s_Data->QuadMaterial, s_Data->QuadPipeline, s_Data->QuadIndexCount);
			s_Data->RendererStats.DrawCalls++;
		}

		if (s_Data->CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->CircleVertexBufferPtr - (uint8_t*)s_Data->CircleVertexBufferBase);
			s_Data->CircleVertexBuffer->SetData(s_Data->CircleVertexBufferBase, dataSize);

			// Use quad QuadIndexBuffer
			Renderer::DrawIndexed(s_Data->CircleVertexBuffer, s_Data->QuadIndexBuffer, s_Data->CircleMaterial, s_Data->CirclePipeline, s_Data->CircleIndexCount);
			s_Data->RendererStats.DrawCalls++;
		}

		if (s_Data->LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->LineVertexBufferPtr - (uint8_t*)s_Data->LineVertexBufferBase);
			s_Data->LineVertexBuffer->SetData(s_Data->LineVertexBufferBase, dataSize);

			Renderer::DrawLines(s_Data->LineVertexBuffer, s_Data->LineMaterial, s_Data->LinePipeline, s_Data->LineVertexCount);
			s_Data->RendererStats.DrawCalls++;
		}

		if (s_Data->TextIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->TextVertexBufferPtr - (uint8_t*)s_Data->TextVertexBufferBase);
			s_Data->TextVertexBuffer->SetData(s_Data->TextVertexBufferBase, dataSize);

			// Bind textures
			// TODO: Bind multiple font atlas texture
			s_Data->TextMaterial->SetTexture(s_Data->FontAtlasTexture, 0);

			Renderer::DrawIndexed(s_Data->TextVertexBuffer, s_Data->QuadIndexBuffer, s_Data->TextMaterial, s_Data->TextPipeline, s_Data->TextIndexCount);
			s_Data->RendererStats.DrawCalls++;
		}

		Renderer::EndRenderPass(s_Data->TatgetRenderPass);
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data->RendererStats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data->RendererStats;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		s_Data->LineVertexBufferPtr->Position = p0;
		s_Data->LineVertexBufferPtr->Color = color;
		s_Data->LineVertexBufferPtr->EntityID = entityID;
		s_Data->LineVertexBufferPtr++;

		s_Data->LineVertexBufferPtr->Position = p1;
		s_Data->LineVertexBufferPtr->Color = color;
		s_Data->LineVertexBufferPtr->EntityID = entityID;
		s_Data->LineVertexBufferPtr++;

		s_Data->LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color, entityID);
		DrawLine(p1, p2, color, entityID);
		DrawLine(p2, p3, color, entityID);
		DrawLine(p3, p0, color, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Data->QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color, entityID);
		DrawLine(lineVertices[1], lineVertices[2], color, entityID);
		DrawLine(lineVertices[2], lineVertices[3], color, entityID);
		DrawLine(lineVertices[3], lineVertices[0], color, entityID);
	}

	void Renderer2D::DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID)
	{
		const auto& fontGeometry = font->GetMSDFData()->FontGeometry;
		const auto& metrics = fontGeometry.getMetrics();
		Ref<Texture2D> fontAtlas = font->GetAtlasTexture();

		s_Data->FontAtlasTexture = fontAtlas;

		double x = 0.0;
		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
		double y = 0.0;
		const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();


		for (size_t i = 0; i < string.size(); i++)
		{
			char character = string[i];
			if (character == '\r')
				continue;

			if (character == '\n')
			{
				x = 0;
				y -= fsScale * metrics.lineHeight + textParams.LineSpacing;
				continue;
			}

			if (character == ' ')
			{
				float advance = spaceGlyphAdvance;
				if (i < string.size() - 1)
				{
					char nextCharacter = string[i + 1];
					double dAdvance;
					fontGeometry.getAdvance(dAdvance, character, nextCharacter);
					advance = (float)dAdvance;
				}

				x += fsScale * advance + textParams.Kerning;
				continue;
			}

			if (character == '\t')
			{
				// NOTE: is this right?
				x += 4.0f * (fsScale * spaceGlyphAdvance + textParams.Kerning);
				continue;
			}

			auto glyph = fontGeometry.getGlyph(character);
			if (!glyph)
				glyph = fontGeometry.getGlyph('?');
			if (!glyph)
				return;

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			glm::vec2 texCoordMin((float)al, (float)at);
			glm::vec2 texCoordMax((float)ar, (float)ab);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			glm::vec2 quadMin((float)pl, (float)pt);
			glm::vec2 quadMax((float)pr, (float)pb);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / fontAtlas->GetWidth();
			float texelHeight = 1.0f / fontAtlas->GetHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			// render here
			s_Data->TextVertexBufferPtr->Position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
			s_Data->TextVertexBufferPtr->Color = textParams.Color;
			s_Data->TextVertexBufferPtr->TexCoord = texCoordMin;
			s_Data->TextVertexBufferPtr->EntityID = entityID;
			s_Data->TextVertexBufferPtr++;

			s_Data->TextVertexBufferPtr->Position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
			s_Data->TextVertexBufferPtr->Color = textParams.Color;
			s_Data->TextVertexBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
			s_Data->TextVertexBufferPtr->EntityID = entityID;
			s_Data->TextVertexBufferPtr++;

			s_Data->TextVertexBufferPtr->Position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
			s_Data->TextVertexBufferPtr->Color = textParams.Color;
			s_Data->TextVertexBufferPtr->TexCoord = texCoordMax;
			s_Data->TextVertexBufferPtr->EntityID = entityID;
			s_Data->TextVertexBufferPtr++;

			s_Data->TextVertexBufferPtr->Position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
			s_Data->TextVertexBufferPtr->Color = textParams.Color;
			s_Data->TextVertexBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
			s_Data->TextVertexBufferPtr->EntityID = entityID;
			s_Data->TextVertexBufferPtr++;

			s_Data->TextIndexCount += 6;
			s_Data->RendererStats.QuadCount++;

			if (i < string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = string[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance + textParams.Kerning;
			}
		}
	}

	void Renderer2D::DrawString(const glm::mat4& transform, const TextComponent& tc, int entityID)
	{
		DrawString(tc.TextString, tc.FontAsset, transform, { tc.Color, tc.Kerning, tc.LineSpacing }, entityID);
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Data->LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Data->LineWidth = width;
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color,
		float thickness, float fade, int entityID)
	{
		if (s_Data->CircleIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->CircleVertexBufferPtr->WorldPosition = transform * s_Data->QuadVertexPositions[i];
			s_Data->CircleVertexBufferPtr->LocalPosition = s_Data->QuadVertexPositions[i] * 2.0f;
			s_Data->CircleVertexBufferPtr->Color = color;
			s_Data->CircleVertexBufferPtr->Thickness = thickness;
			s_Data->CircleVertexBufferPtr->Fade = fade;
			s_Data->CircleVertexBufferPtr->EntityID = entityID;
			s_Data->CircleVertexBufferPtr++;
		}

		s_Data->CircleIndexCount += 6;

		s_Data->RendererStats.QuadCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.TextureHandle)
		{
			Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(src.TextureHandle);
			DrawQuad(transform, texture, src.UVStart, src.UVEnd, src.Color, src.TilingFactor, entityID);
		}
		else
		{
			DrawQuad(transform, src.Color, entityID);
		}
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, CircleRendererComponent& crc, int entityID)
	{
		DrawCircle(transform, crc.Color, crc.Thickness, crc.Fade, entityID);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		const float texIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		SetQuadVertex(transform, color, entityID, s_Data->QuadTexCoord, texIndex, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, glm::vec2 uv0, glm::vec2 uv1,
		const glm::vec4& tintColor, float tilingFactor, int entityID)
	{
		if (s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		HNB_CORE_VERIFY(texture);

		glm::vec2 textureCoords[] = { uv0, { uv1.x, uv0.y }, uv1, { uv0.x, uv1.y } };
		SetQuadVertex(transform, tintColor, entityID, textureCoords, GetTextureID(texture), tilingFactor);
	}

	float Renderer2D::GetTextureID(const Ref<Texture2D>& texture)
	{
		float texIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data->TextureSlotIndex; i++)
		{
			if (*s_Data->TextureSlots[i] == *texture)
			{
				texIndex = (float)i;
				break;
			}
		}

		if (texIndex == 0.0f)
		{
			if (s_Data->TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();
			texIndex = (float)s_Data->TextureSlotIndex;
			s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
			s_Data->TextureSlotIndex++;
		}
		return texIndex;
	}

	void Renderer2D::SetQuadVertex(const glm::mat4& transform,
		const glm::vec4& color, int entityID, const glm::vec2* texCoord, float texIndex, float tilingFactor)
	{
		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[i];
			s_Data->QuadVertexBufferPtr->Color = color;
			s_Data->QuadVertexBufferPtr->TexCoord = texCoord[i];
			s_Data->QuadVertexBufferPtr->TexIndex = texIndex;
			s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data->QuadVertexBufferPtr->EntityID = entityID;
			s_Data->QuadVertexBufferPtr++;
		}
		s_Data->QuadIndexCount += 6;
		s_Data->RendererStats.QuadCount++;
	}
}