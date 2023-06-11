#include "hnbpch.h"
#include "OpenGLAPI.h"

#include <glad/glad.h>

namespace Hanabi
{
	static GLenum PrimitiveTopologyTypeToOpenGL(PrimitiveTopology type)
	{
		switch (type)
		{
		case PrimitiveTopology::Points:
			return GL_POINT;
		case PrimitiveTopology::Lines:
			return GL_LINES;
		case PrimitiveTopology::Triangles:
			return GL_TRIANGLES;
		}

		HNB_CORE_ASSERT(false, "Unknown Primitive Topology!");
	}

	void OpenGLMessageCallback(unsigned source, unsigned type, unsigned id, 
		unsigned severity, int length, const char* message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         HNB_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       HNB_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          HNB_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: HNB_CORE_TRACE(message); return;
		}

		HNB_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
#ifdef HNB_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::BeginRender()
	{}

	void OpenGLRendererAPI::EndRender()
	{}

	void OpenGLRendererAPI::BeginRenderPass(const Ref<RenderPass> renderPass)
	{}

	void OpenGLRendererAPI::EndRenderPass(const Ref<RenderPass> renderPass)
	{}

	void OpenGLRendererAPI::ClearAndBind()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SubmitStaticMesh(const Ref<Mesh> mesh, const Ref<Pipeline> pipeline)
	{}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexBuffer> vertexBuffer, const Ref<IndexBuffer> indexBuffer, const Ref<Pipeline> pipeline, uint32_t indexCount)
	{
		vertexBuffer->Bind();
		indexBuffer->Bind();
		pipeline->Bind();
		pipeline->GetSpecification().Shader->Bind();

		uint32_t count = indexCount ? indexCount : indexBuffer->GetCount();
		glDrawElements(PrimitiveTopologyTypeToOpenGL(pipeline->GetSpecification().Topology), count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexBuffer> vertexBuffer, const Ref<Pipeline> pipeline, uint32_t vertexCount)
	{
		vertexBuffer->Bind();
		pipeline->Bind();
		pipeline->GetSpecification().Shader->Bind();

		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}
}