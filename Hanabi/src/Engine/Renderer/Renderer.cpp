#include "hnbpch.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"
#include "MeshFactory.h"

namespace Hanabi
{
	static Scope<RendererAPI> s_RendererAPI = nullptr;

	struct RendererData
	{
		Ref<ShaderLibrary> ShaderLibrary;

		Ref<Texture2D> WhiteTexture;

		Ref<StaticMesh> CubeMesh;
		Ref<StaticMesh> SphereMesh;
		Ref<StaticMesh> CapsuleMesh;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Init()
	{
		s_Data = new RendererData();
		s_RendererAPI = RendererAPI::Create();
		s_RendererAPI->Init();

		//TODO:Load shaders
		//s_Data->ShaderLibrary

		//Setup textures
		s_Data->WhiteTexture = Texture2D::Create(TextureSpecification());
		uint32_t whiteTextureData = 0xffffffff;
		s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		//Load default meshes
		s_Data->CubeMesh = MeshFactory::CreateBox(glm::vec3(1.0f));
		//s_Data->CapsuleMesh = MeshFactory::CreateCapsule(1.0f, 1.0f);
		s_Data->SphereMesh = MeshFactory::CreateSphere(1.0f);
	}

	void Renderer::Shutdown()
	{}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		s_RendererAPI->SetClearColor(color);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(0, 0, width, height);
	}

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass)
	{
		s_RendererAPI->BeginRenderPass(renderPass);
	}

	void Renderer::EndRenderPass(const Ref<RenderPass>& renderPass)
	{
		s_RendererAPI->EndRenderPass(renderPass);
	}

	void Renderer::ResetToSwapChain()
	{
		s_RendererAPI->ResetToSwapChain();
	}

	void Renderer::SubmitStaticMesh(const Ref<StaticMesh>& mesh,const Ref<Pipeline>& pipeline)
	{
		s_RendererAPI->SubmitStaticMesh(mesh, pipeline);
	}

	void  Renderer::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
		const Ref<Pipeline>& pipeline, uint32_t indexCount)
	{
		s_RendererAPI->DrawIndexed(vertexBuffer, indexBuffer, material, pipeline, indexCount);
	}

	void  Renderer::DrawLines(const Ref<VertexBuffer>& vertexBuffer, const Ref<Material>& material, const Ref<Pipeline>& pipeline, uint32_t vertexCount)
	{
		s_RendererAPI->DrawLines(vertexBuffer, material, pipeline, vertexCount);
	}

	Ref<Texture2D> Renderer::GetWhiteTexture()
	{
		return s_Data->WhiteTexture;
	}

	Ref<StaticMesh> Renderer::GetCubeMesh()
	{
		return s_Data->CubeMesh;
	}

	Ref<StaticMesh> Renderer::GetSphereMesh()
	{
		return s_Data->SphereMesh;
	}

	Ref<StaticMesh> Renderer::GetCapsuleMesh()
	{
		return s_Data->CapsuleMesh;
	}
}