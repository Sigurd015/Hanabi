#include "hnbpch.h"
#include "OpenGLPipeline.h"

#include <glad/glad.h>

namespace Hanabi
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
		}
		HNB_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
	{
		HNB_CORE_ASSERT(spec.Layout.GetElements().size(), "Vertex Buffer has no layout!");

		m_Specification = spec;
		glCreateVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);
		size_t index = 0;
		for (const auto& element : m_Specification.Layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glEnableVertexAttribArray(index);
				glVertexAttribPointer(index,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					m_Specification.Layout.GetStride(),
					(const void*)element.Offset);
				index++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(index);
				glVertexAttribIPointer(index,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					m_Specification.Layout.GetStride(),
					(const void*)element.Offset);
				index++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(index);
					glVertexAttribPointer(index,
						count,
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						m_Specification.Layout.GetStride(),
						(const void*)(element.Offset + sizeof(float) * count * i));
					glVertexAttribDivisor(index, 1);
					index++;
				}
				break;
			}
			default:
				HNB_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLPipeline::Bind()
	{
		glBindVertexArray(m_RendererID);
		if (m_UniformBuffers.size())
		{
			for (auto& constantBuffer : m_UniformBuffers)
			{
				constantBuffer->Bind();
			}
		}
	}

	void OpenGLPipeline::SetConstantBuffer(const Ref<ConstantBuffer>& uniformBuffer)
	{
		m_UniformBuffers.push_back(uniformBuffer);
	}

	Ref<ConstantBuffer> OpenGLPipeline::GetConstantBuffer(uint32_t bindingID)
	{
		HNB_CORE_ASSERT(bindingID < m_UniformBuffers.size(), "Binding ID is out of range!");
		return m_UniformBuffers[bindingID];
	}
}