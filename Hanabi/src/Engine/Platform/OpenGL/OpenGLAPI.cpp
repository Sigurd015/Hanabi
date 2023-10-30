#include "hnbpch.h"
#include "OpenGLAPI.h"
#include "Engine/Renderer/Renderer.h"

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

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
		glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
	}

	void OpenGLRendererAPI::Clear()
	{
		static const glm::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear)
	{
		renderPass->GetTargetFramebuffer()->Bind();
		if (clear)
		{
			Clear();
			renderPass->GetTargetFramebuffer()->ClearAttachment();
		}
		//glLineWidth(pipeline->GetSpecification().LineWidth);
	}

	void OpenGLRendererAPI::EndRenderPass()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLRendererAPI::DrawMesh(const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		material->Bind();

		DrawMesh(mesh);
	}

	void OpenGLRendererAPI::DrawMesh(const Ref<Mesh>& mesh)
	{
		Ref<MeshSource> source = mesh->GetMeshSource();
		source->GetVertexBuffer()->Bind();
		source->GetIndexBuffer()->Bind();

		//glDrawElements(PrimitiveTopologyTypeToOpenGL(pipeline->GetSpecification().Topology), source->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material, uint32_t indexCount)
	{
		material->Bind();
		DrawIndexed(vertexBuffer, indexBuffer, indexCount);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t indexCount)
	{
		vertexBuffer->Bind();
		indexBuffer->Bind();

		//glDrawElements(PrimitiveTopologyTypeToOpenGL(pipeline->GetSpecification().Topology), indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount)
	{
		vertexBuffer->Bind();

		//glDrawArrays(PrimitiveTopologyTypeToOpenGL(pipeline->GetSpecification().Topology), 0, vertexCount);
	}

	std::pair<Ref<TextureCube>, Ref<TextureCube>> OpenGLRendererAPI::CreateEnvironmentMap(const Ref<Texture2D>& equirectangularMap)
	{
		if (!Renderer::GetConfig().ComputeEnvironmentMaps)
			return { Renderer::GetTexture<TextureCube>("BlackCube"), Renderer::GetTexture<TextureCube>("BlackCube") };

		return { Renderer::GetTexture<TextureCube>("BlackCube"), Renderer::GetTexture<TextureCube>("BlackCube") };
	}
}