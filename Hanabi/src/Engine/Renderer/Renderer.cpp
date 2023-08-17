#include "hnbpch.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"
#include "MeshFactory.h"
#include "SceneRenderer.h"

namespace Hanabi
{
	static Scope<RendererAPI> s_RendererAPI = nullptr;

	struct RendererData
	{
		Ref<ShaderLibrary> ShaderLibrary;

		std::unordered_map<std::string, Ref<Texture2D>> Textures;
		std::unordered_map<std::string, Ref<Mesh>> Meshes;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Init()
	{
		s_Data = new RendererData();
		s_RendererAPI = RendererAPI::Create();
		s_RendererAPI->Init();

		//TODO:Load shaders from spirv files
		s_Data->ShaderLibrary = CreateRef<ShaderLibrary>();
		s_Data->ShaderLibrary->Load("Renderer2D_Quad");
		s_Data->ShaderLibrary->Load("Renderer2D_Circle");
		s_Data->ShaderLibrary->Load("Renderer2D_Line");
		s_Data->ShaderLibrary->Load("Renderer2D_Text");
		s_Data->ShaderLibrary->Load("PhongLighting");

		//Setup textures
		Ref<Texture2D> whiteTexture = Texture2D::Create(TextureSpecification());
		uint32_t whiteTextureData = 0xffffffff;
		whiteTexture->SetData(Buffer(&whiteTextureData, sizeof(uint32_t)));
		s_Data->Textures["White"] = whiteTexture;

		//Load default meshes
		s_Data->Meshes["Box"] = MeshFactory::CreateBox({ 1.0f,1.0f,1.0f });
		//s_Data->Meshes["Capsule"] = MeshFactory::CreateCapsule(1.0f, 1.0f);
		//s_Data->Meshes["Sphere"] = MeshFactory::CreateSphere(1.0f);

		Renderer2D::Init();
		SceneRenderer::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		SceneRenderer::Shutdown();
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		s_RendererAPI->SetClearColor(color);
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(0, 0, width, height);
	}

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear)
	{
		s_RendererAPI->BeginRenderPass(renderPass, clear);
	}

	void Renderer::EndRenderPass(const Ref<RenderPass>& renderPass)
	{
		s_RendererAPI->EndRenderPass(renderPass);
	}

	void Renderer::SubmitStaticMesh(const Ref<Mesh>& mesh, const Ref<Material>& material, const Ref<Pipeline>& pipeline)
	{
		s_RendererAPI->SubmitStaticMesh(mesh, material, pipeline);
	}

	void Renderer::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
		const Ref<Pipeline>& pipeline, uint32_t indexCount)
	{
		s_RendererAPI->DrawIndexed(vertexBuffer, indexBuffer, material, pipeline, indexCount);
	}

	void Renderer::DrawLines(const Ref<VertexBuffer>& vertexBuffer, const Ref<Material>& material, const Ref<Pipeline>& pipeline, uint32_t vertexCount)
	{
		s_RendererAPI->DrawLines(vertexBuffer, material, pipeline, vertexCount);
	}

	Ref<Shader> Renderer::GetShader(const std::string& name)
	{
		return s_Data->ShaderLibrary->Get(name);
	}

	Ref<Mesh> Renderer::GetMesh(const std::string& name)
	{
		if (s_Data->Meshes.find(name) == s_Data->Meshes.end())
			return nullptr;

		return s_Data->Meshes[name];
	}

	Ref<Texture2D> Renderer::GetTexture(const std::string& name)
	{
		if (s_Data->Textures.find(name) == s_Data->Textures.end())
			return nullptr;

		return s_Data->Textures[name];
	}

	Ref<Shader> Renderer::GetDefaultShader()
	{
		return s_Data->ShaderLibrary->Get("PhongLighting");
	}
}