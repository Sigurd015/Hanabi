#include "hnbpch.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Pipeline.h"
#include "Shader.h"
#include "Renderer.h"
#include "ConstantBuffer.h"
#include "UI/MSDFData.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hanabi
{
	static Renderer2D::Statistics RendererStats;

	void Renderer2D::Init()
	{
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
			m_Data.QuadVertexBuffer = VertexBuffer::Create(m_Data.MaxVertices * sizeof(QuadVertex));
			m_Data.QuadVertexBuffer->SetLayout(layout);
			
			uint32_t* indices = new uint32_t[m_Data.MaxIndices];
			uint32_t offset = 0;
			for (uint32_t i = 0; i < m_Data.MaxIndices; i += 6)
			{
				indices[i + 0] = offset + 2;
				indices[i + 1] = offset + 1;
				indices[i + 2] = offset + 0;

				indices[i + 3] = offset + 0;
				indices[i + 4] = offset + 3;
				indices[i + 5] = offset + 2;

				offset += 4;
			}
			m_Data.QuadIndexBuffer = IndexBuffer::Create(indices, m_Data.MaxIndices);
			delete[] indices;

			PipelineSpecification pipelineSpec;
			pipelineSpec.Layout = layout;
			pipelineSpec.RenderPass = nullptr;
			pipelineSpec.Shader = Shader::Create("Renderer2D_Quad");
			pipelineSpec.Topology = PrimitiveTopology::Triangles;

			// For OpenGL
			pipelineSpec.VertexBuffer = m_Data.QuadVertexBuffer;
			pipelineSpec.IndexBuffer = m_Data.QuadIndexBuffer;
			//-----------------

			m_Data.QuadPipeline = Pipeline::Create(pipelineSpec);
			m_Data.QuadMaterial = Material::Create(pipelineSpec.Shader);

			m_Data.QuadVertexBufferBase = new QuadVertex[m_Data.MaxVertices];
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
			m_Data.CircleVertexBuffer = VertexBuffer::Create(m_Data.MaxVertices * sizeof(CircleVertex));
			m_Data.CircleVertexBuffer->SetLayout(layout);

			m_Data.CircleIndexBuffer = m_Data.QuadIndexBuffer;

			PipelineSpecification pipelineSpec;
			pipelineSpec.Layout = layout;
			pipelineSpec.RenderPass = nullptr;
			pipelineSpec.Shader = Shader::Create("Renderer2D_Circle");
			pipelineSpec.Topology = PrimitiveTopology::Triangles;

			// For OpenGL
			pipelineSpec.VertexBuffer = m_Data.CircleVertexBuffer;
			pipelineSpec.IndexBuffer = m_Data.CircleIndexBuffer;
			//-----------------

			m_Data.CirclePipeline = Pipeline::Create(pipelineSpec);
			m_Data.CircleMaterial = Material::Create(pipelineSpec.Shader);

			m_Data.CircleVertexBufferBase = new CircleVertex[m_Data.MaxVertices];
		}

		// Lines
		{
			VertexBufferLayout layout = {
			  { ShaderDataType::Float3, "a_Position" },
			  { ShaderDataType::Float4, "a_Color"    },
			  { ShaderDataType::Int,    "a_EntityID" }
			};
			m_Data.LineVertexBuffer = VertexBuffer::Create(m_Data.MaxVertices * sizeof(LineVertex));
			m_Data.LineVertexBuffer->SetLayout(layout);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Layout = layout;
			pipelineSpec.RenderPass = nullptr;
			pipelineSpec.Shader = Shader::Create("Renderer2D_Line");
			pipelineSpec.Topology = PrimitiveTopology::Lines;

			// For OpenGL
			pipelineSpec.VertexBuffer = m_Data.LineVertexBuffer;
			//-----------------

			m_Data.LinePipeline = Pipeline::Create(pipelineSpec);
			m_Data.LineMaterial = Material::Create(pipelineSpec.Shader);

			m_Data.LineVertexBufferBase = new LineVertex[m_Data.MaxVertices];
		}

		// Text
		{
			VertexBufferLayout layout = {
			  { ShaderDataType::Float3, "a_Position"     },
			  { ShaderDataType::Float4, "a_Color"        },
			  { ShaderDataType::Float2, "a_TexCoord"     },
			  { ShaderDataType::Int,    "a_EntityID"     }
			};
			m_Data.TextVertexBuffer = VertexBuffer::Create(m_Data.MaxVertices * sizeof(TextVertex));
			m_Data.TextVertexBuffer->SetLayout(layout);

			uint32_t* indices = new uint32_t[m_Data.MaxIndices];
			uint32_t offset = 0;
			for (uint32_t i = 0; i < m_Data.MaxIndices; i += 6)
			{
				indices[i + 0] = offset + 0;
				indices[i + 1] = offset + 1;
				indices[i + 2] = offset + 2;

				indices[i + 3] = offset + 2;
				indices[i + 4] = offset + 3;
				indices[i + 5] = offset + 0;

				offset += 4;
			}
			m_Data.TextIndexBuffer = IndexBuffer::Create(indices, m_Data.MaxIndices);
			delete[] indices;

			PipelineSpecification pipelineSpec;
			pipelineSpec.Layout = layout;
			pipelineSpec.RenderPass = nullptr;
			pipelineSpec.Shader = Shader::Create("Renderer2D_Text");
			pipelineSpec.Topology = PrimitiveTopology::Triangles;

			// For OpenGL
			pipelineSpec.VertexBuffer = m_Data.TextVertexBuffer;
			pipelineSpec.IndexBuffer = m_Data.TextIndexBuffer;
			//-----------------

			m_Data.TextPipeline = Pipeline::Create(pipelineSpec);
			m_Data.TextMaterial = Material::Create(pipelineSpec.Shader);

			m_Data.TextVertexBufferBase = new TextVertex[m_Data.MaxVertices];
		}

		// Set WhiteTexture slots to 0
		m_Data.WhiteTexture = Renderer::GetWhiteTexture();
		m_Data.TextureSlots[0] = m_Data.WhiteTexture;

		m_Data.CameraConstantBuffer = ConstantBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		delete[] m_Data.QuadVertexBufferBase;
		delete[] m_Data.CircleVertexBufferBase;
		delete[] m_Data.LineVertexBufferBase;
		delete[] m_Data.TextVertexBufferBase;
	}

	void Renderer2D::SetViewProjection(const glm::mat4& viewProjection)
	{
		m_Data.CameraBuffer.ViewProjection = viewProjection;
		m_Data.CameraConstantBuffer->SetData(&m_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		m_Data.QuadPipeline->SetConstantBuffer(m_Data.CameraConstantBuffer);
		m_Data.CirclePipeline->SetConstantBuffer(m_Data.CameraConstantBuffer);
		m_Data.LinePipeline->SetConstantBuffer(m_Data.CameraConstantBuffer);
		m_Data.TextPipeline->SetConstantBuffer(m_Data.CameraConstantBuffer);

		StartBatch();
	}

	Ref<RenderPass> Renderer2D::GetTargetRenderPass()
	{
		return m_Data.QuadPipeline->GetSpecification().RenderPass;
	}

	void Renderer2D::SetTargetRenderPass(const Ref<RenderPass>& renderPass)
	{
		if (renderPass != m_Data.QuadPipeline->GetSpecification().RenderPass)
		{
			{
				PipelineSpecification pipelineSpecification = m_Data.QuadPipeline->GetSpecification();
				pipelineSpecification.RenderPass = renderPass;
				m_Data.QuadPipeline = Pipeline::Create(pipelineSpecification);
			}

			{
				PipelineSpecification pipelineSpecification = m_Data.CirclePipeline->GetSpecification();
				pipelineSpecification.RenderPass = renderPass;
				m_Data.CirclePipeline = Pipeline::Create(pipelineSpecification);
			}

			{
				PipelineSpecification pipelineSpecification = m_Data.LinePipeline->GetSpecification();
				pipelineSpecification.RenderPass = renderPass;
				m_Data.LinePipeline = Pipeline::Create(pipelineSpecification);
			}

			{
				PipelineSpecification pipelineSpecification = m_Data.TextPipeline->GetSpecification();
				pipelineSpecification.RenderPass = renderPass;
				m_Data.TextPipeline = Pipeline::Create(pipelineSpecification);
			}
		}
	}

	void Renderer2D::StartBatch()
	{
		ResetStats();

		m_Data.QuadIndexCount = 0;
		m_Data.QuadVertexBufferPtr = m_Data.QuadVertexBufferBase;

		m_Data.CircleIndexCount = 0;
		m_Data.CircleVertexBufferPtr = m_Data.CircleVertexBufferBase;

		m_Data.LineVertexCount = 0;
		m_Data.LineVertexBufferPtr = m_Data.LineVertexBufferBase;

		m_Data.TextIndexCount = 0;
		m_Data.TextVertexBufferPtr = m_Data.TextVertexBufferBase;

		m_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		if (m_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.QuadVertexBufferPtr - (uint8_t*)m_Data.QuadVertexBufferBase);
			m_Data.QuadVertexBuffer->SetData(m_Data.QuadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < m_Data.TextureSlotIndex; i++)
				m_Data.QuadMaterial->SetTexture(m_Data.TextureSlots[i], i);

			Renderer::DrawIndexed(m_Data.QuadVertexBuffer, m_Data.QuadIndexBuffer, m_Data.QuadMaterial, m_Data.QuadPipeline, m_Data.QuadIndexCount);
			RendererStats.DrawCalls++;
		}

		if (m_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.CircleVertexBufferPtr - (uint8_t*)m_Data.CircleVertexBufferBase);
			m_Data.CircleVertexBuffer->SetData(m_Data.CircleVertexBufferBase, dataSize);

			// Use quad QuadIndexBuffer
			Renderer::DrawIndexed(m_Data.CircleVertexBuffer, m_Data.CircleIndexBuffer, m_Data.CircleMaterial, m_Data.CirclePipeline, m_Data.CircleIndexCount);
			RendererStats.DrawCalls++;
		}

		if (m_Data.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.LineVertexBufferPtr - (uint8_t*)m_Data.LineVertexBufferBase);
			m_Data.LineVertexBuffer->SetData(m_Data.LineVertexBufferBase, dataSize);

			Renderer::DrawLines(m_Data.LineVertexBuffer, m_Data.LineMaterial, m_Data.LinePipeline, m_Data.LineVertexCount);
			RendererStats.DrawCalls++;
		}

		if (m_Data.TextIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)m_Data.TextVertexBufferPtr - (uint8_t*)m_Data.TextVertexBufferBase);
			m_Data.TextVertexBuffer->SetData(m_Data.TextVertexBufferBase, dataSize);

			// Bind textures
			// TODO: Bind multiple font atlas texture
			m_Data.TextMaterial->SetTexture(m_Data.FontAtlasTexture, 0);

			Renderer::DrawIndexed(m_Data.TextVertexBuffer, m_Data.TextIndexBuffer, m_Data.TextMaterial, m_Data.TextPipeline, m_Data.TextIndexCount);
			RendererStats.DrawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::ResetStats()
	{
		memset(&RendererStats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return RendererStats;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		m_Data.LineVertexBufferPtr->Position = p0;
		m_Data.LineVertexBufferPtr->Color = color;
		m_Data.LineVertexBufferPtr->EntityID = entityID;
		m_Data.LineVertexBufferPtr++;

		m_Data.LineVertexBufferPtr->Position = p1;
		m_Data.LineVertexBufferPtr->Color = color;
		m_Data.LineVertexBufferPtr->EntityID = entityID;
		m_Data.LineVertexBufferPtr++;

		m_Data.LineVertexCount += 2;
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
			lineVertices[i] = transform * m_Data.QuadVertexPositions[i];

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

		m_Data.FontAtlasTexture = fontAtlas;

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
			glm::vec2 texCoordMin((float)al, (float)ab);
			glm::vec2 texCoordMax((float)ar, (float)at);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			glm::vec2 quadMin((float)pl, (float)pb);
			glm::vec2 quadMax((float)pr, (float)pt);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / fontAtlas->GetWidth();
			float texelHeight = 1.0f / fontAtlas->GetHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			// render here
			m_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
			m_Data.TextVertexBufferPtr->Color = textParams.Color;
			m_Data.TextVertexBufferPtr->TexCoord = texCoordMin;
			m_Data.TextVertexBufferPtr->EntityID = entityID;
			m_Data.TextVertexBufferPtr++;

			m_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
			m_Data.TextVertexBufferPtr->Color = textParams.Color;
			m_Data.TextVertexBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
			m_Data.TextVertexBufferPtr->EntityID = entityID;
			m_Data.TextVertexBufferPtr++;

			m_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
			m_Data.TextVertexBufferPtr->Color = textParams.Color;
			m_Data.TextVertexBufferPtr->TexCoord = texCoordMax;
			m_Data.TextVertexBufferPtr->EntityID = entityID;
			m_Data.TextVertexBufferPtr++;

			m_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
			m_Data.TextVertexBufferPtr->Color = textParams.Color;
			m_Data.TextVertexBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
			m_Data.TextVertexBufferPtr->EntityID = entityID;
			m_Data.TextVertexBufferPtr++;

			m_Data.TextIndexCount += 6;
			RendererStats.QuadCount++;

			if (i < string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = string[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance + textParams.Kerning;
			}
		}
	}

	void Renderer2D::DrawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entityID)
	{
		DrawString(string, component.FontAsset, transform, { component.Color, component.Kerning, component.LineSpacing }, entityID);
	}

	float Renderer2D::GetLineWidth()
	{
		return m_Data.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		m_Data.LineWidth = width;
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color,
		float thickness, float fade, int entityID)
	{
		if (m_Data.CircleIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < 4; i++)
		{
			m_Data.CircleVertexBufferPtr->WorldPosition = transform * m_Data.QuadVertexPositions[i];
			m_Data.CircleVertexBufferPtr->LocalPosition = m_Data.QuadVertexPositions[i] * 2.0f;
			m_Data.CircleVertexBufferPtr->Color = color;
			m_Data.CircleVertexBufferPtr->Thickness = thickness;
			m_Data.CircleVertexBufferPtr->Fade = fade;
			m_Data.CircleVertexBufferPtr->EntityID = entityID;
			m_Data.CircleVertexBufferPtr++;
		}

		m_Data.CircleIndexCount += 6;

		RendererStats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		if (m_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		const float texIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		SetQuadVertex(transform, color, entityID, m_Data.QuadTexCoord, texIndex, tilingFactor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, glm::vec2 uv0, glm::vec2 uv1,
		const glm::vec4& tintColor, float tilingFactor, int entityID)
	{
		if (m_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		glm::vec2 textureCoords[] = { uv0, { uv1.x, uv0.y }, uv1, { uv0.x, uv1.y } };
		SetQuadVertex(transform, tintColor, entityID, textureCoords, GetTextureID(texture), tilingFactor);
	}

	float Renderer2D::GetTextureID(const Ref<Texture2D>& texture)
	{
		float texIndex = 0.0f;
		for (uint32_t i = 1; i < m_Data.TextureSlotIndex; i++)
		{
			if (*m_Data.TextureSlots[i] == *texture)
			{
				texIndex = (float)i;
				break;
			}
		}

		if (texIndex == 0.0f)
		{
			if (m_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();
			texIndex = (float)m_Data.TextureSlotIndex;
			m_Data.TextureSlots[m_Data.TextureSlotIndex] = texture;
			m_Data.TextureSlotIndex++;
		}
		return texIndex;
	}

	void Renderer2D::SetQuadVertex(const glm::mat4& transform,
		const glm::vec4& color, int entityID, const glm::vec2* texCoord, float texIndex, float tilingFactor)
	{
		for (size_t i = 0; i < 4; i++)
		{
			m_Data.QuadVertexBufferPtr->Position = transform * m_Data.QuadVertexPositions[i];
			m_Data.QuadVertexBufferPtr->Color = color;
			m_Data.QuadVertexBufferPtr->TexCoord = texCoord[i];
			m_Data.QuadVertexBufferPtr->TexIndex = texIndex;
			m_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			m_Data.QuadVertexBufferPtr->EntityID = entityID;
			m_Data.QuadVertexBufferPtr++;
		}
		m_Data.QuadIndexCount += 6;
		RendererStats.QuadCount++;
	}
}