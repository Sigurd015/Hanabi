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

		//TODO: Same index buffer, get a different result
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
		glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		m_ClearColor = color;
	}

	void OpenGLRendererAPI::Clear()
	{
		glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear)
	{
		renderPass->GetSpecification().TargetFramebuffer->Bind();
		if (clear)
		{
			const glm::vec4& clearColor = renderPass->GetSpecification().TargetFramebuffer->GetSpecification().ClearColor;
			SetClearColor(clearColor);
			Clear();
		}
	}

	void OpenGLRendererAPI::EndRenderPass(const Ref<RenderPass>& renderPass)
	{
		renderPass->GetSpecification().TargetFramebuffer->Unbind();
	}

	void OpenGLRendererAPI::SubmitStaticMesh(const Ref<Mesh>& mesh, const Ref<Material>& material, const Ref<Pipeline>& pipeline, const glm::mat4& transform)
	{
		mesh->GetVertexBuffer()->Bind();
		Ref<ConstantBuffer> transformBuffer = pipeline->GetConstantBuffer(1);// 1 is the slot for the transform ConstantBuffer by default
		transformBuffer->SetData(&transform);
		pipeline->Bind();	
		mesh->GetIndexBuffer()->Bind();
		material->Bind();

		glDrawElements(PrimitiveTopologyTypeToOpenGL(pipeline->GetSpecification().Topology), mesh->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material, const Ref<Pipeline>& pipeline, uint32_t indexCount)
	{
		vertexBuffer->Bind();
		pipeline->Bind();
		indexBuffer->Bind();
		material->Bind();

		glDrawElements(PrimitiveTopologyTypeToOpenGL(pipeline->GetSpecification().Topology), indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexBuffer>& vertexBuffer, const Ref<Material>& material, const Ref<Pipeline>& pipeline, uint32_t vertexCount)
	{
		vertexBuffer->Bind();
		pipeline->Bind();
		material->Bind();

		glLineWidth(pipeline->GetSpecification().LineWidth);
		glDrawArrays(PrimitiveTopologyTypeToOpenGL(pipeline->GetSpecification().Topology), 0, vertexCount);
	}
}