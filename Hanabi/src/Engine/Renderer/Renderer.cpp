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

		RenderCommandQueue* CommandQueue;

		std::unordered_map<std::string, Ref<Texture2D>> Textures;
		std::unordered_map<std::string, Ref<Mesh>> Meshes;

		Ref<Material> DefaultMaterial;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Init()
	{
		s_Data = new RendererData();
		s_Data->CommandQueue = new RenderCommandQueue();
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

		//Setup default material
		s_Data->DefaultMaterial = CreateRef<Material>(s_Data->ShaderLibrary->Get("PhongLighting"));
		s_Data->DefaultMaterial->SetTexture(s_Data->Textures["White"], static_cast<uint32_t>(MaterialAsset::TextureType::Diffuse));
		s_Data->DefaultMaterial->SetTexture(s_Data->Textures["White"], static_cast<uint32_t>(MaterialAsset::TextureType::Specular));

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
		Renderer::Submit([renderPass, clear]()
			{
				s_RendererAPI->BeginRenderPass(renderPass, clear);
			});
	}

	void Renderer::EndRenderPass(const Ref<RenderPass>& renderPass)
	{
		Renderer::Submit([renderPass]()
			{
				s_RendererAPI->EndRenderPass(renderPass);
			});
	}

	void Renderer::SubmitStaticMesh(const Ref<Mesh>& mesh, const Ref<Material>& material, const Ref<Pipeline>& pipeline, const void* modelData, uint32_t modelCBBingID)
	{
		Renderer::Submit([mesh, material, pipeline, modelData, modelCBBingID]()
			{
				s_RendererAPI->SubmitStaticMesh(mesh, material, pipeline, modelData, modelCBBingID);
			});
	}

	void Renderer::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
		const Ref<Pipeline>& pipeline, uint32_t indexCount)
	{
		Renderer::Submit([vertexBuffer, indexBuffer, material, pipeline, indexCount]()
			{
				s_RendererAPI->DrawIndexed(vertexBuffer, indexBuffer, material, pipeline, indexCount);
			});
	}

	void Renderer::DrawLines(const Ref<VertexBuffer>& vertexBuffer, const Ref<Material>& material, const Ref<Pipeline>& pipeline, uint32_t vertexCount)
	{
		Renderer::Submit([vertexBuffer, material, pipeline, vertexCount]()
			{
				s_RendererAPI->DrawLines(vertexBuffer, material, pipeline, vertexCount);
			});
	}

	void Renderer::WaitAndRender()
	{
		s_Data->CommandQueue->Execute();
	}

	RenderCommandQueue& Renderer::GetRenderCommandQueue()
	{
		return *s_Data->CommandQueue;
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

	Ref<Material> Renderer::GetDefaultMaterial()
	{
		return s_Data->DefaultMaterial;
	}
}