#include "hnbpch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Hanabi
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size) :m_DataSize(size)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, m_DataSize, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::Bind(uint32_t binding) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t offset)
	{
		glNamedBufferSubData(m_RendererID, offset, m_DataSize, data);
	}
}