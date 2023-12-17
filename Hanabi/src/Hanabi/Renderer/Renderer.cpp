#include "hnbpch.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"
#include "MeshFactory.h"
#include "SceneRenderer.h"
#include "Hanabi/Asset/AssetSerializer/AssetSerializer.h"

namespace Hanabi
{
	static Scope<RendererAPI> s_RendererAPI = nullptr;

	static RendererConfig s_Config;

	struct RendererData
	{
		Ref<ShaderLibrary> ShaderLibrary;

		std::unordered_map<std::string, Ref<Texture>> Textures;
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
		s_Data->ShaderLibrary->Load("DeferredGeometry");
		s_Data->ShaderLibrary->Load("DeferredLighting");
		s_Data->ShaderLibrary->Load("Composite");
		s_Data->ShaderLibrary->Load("DirShadowMap");
		s_Data->ShaderLibrary->Load("Skybox");
		s_Data->ShaderLibrary->Load("EquirectangularToCubemap");
		s_Data->ShaderLibrary->Load("EnvironmentMipFilter");
		s_Data->ShaderLibrary->Load("EnvironmentIrradiance");
		s_Data->ShaderLibrary->Load("PointShadowMap");

		//Setup textures
		TextureSpecification spec;
		spec.Format = ImageFormat::RGBA;
		spec.Width = 1;
		spec.Height = 1;
		spec.GenerateMips = false;

		constexpr uint32_t whiteTextureData = 0xffffffff;
		s_Data->Textures["White"] = Texture2D::Create(spec, Buffer(&whiteTextureData, sizeof(uint32_t)));
		constexpr uint32_t blackTextureData = 0xff000000;
		s_Data->Textures["Black"] = Texture2D::Create(spec, Buffer(&blackTextureData, sizeof(uint32_t)));

		constexpr uint32_t blackCubeTextureData[6] = { 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000 };
		s_Data->Textures["BlackCube"] = TextureCube::Create(spec, Buffer(blackCubeTextureData, sizeof(blackCubeTextureData)));

		// Notice: format, width and height are automatically set by TextureSerializer::LoadTexture2D, so we can reuse spec
		s_Data->Textures["BRDFLut"] = TextureSerializer::LoadTexture2D("Resources/Renderer/BRDF_LUT.png", spec);

		//Load default meshes
		s_Data->Meshes["Box"] = MeshFactory::CreateBox({ 1.0f,1.0f,1.0f });
		s_Data->Meshes["Capsule"] = MeshFactory::CreateCapsule(1.0f, 1.0f);
		s_Data->Meshes["Sphere"] = MeshFactory::CreateSphere(1.0f);

		Renderer2D::Init();
		SceneRenderer::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		SceneRenderer::Shutdown();

		delete s_Data;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(width, height);
	}

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear)
	{
		s_RendererAPI->BeginRenderPass(renderPass, clear);
	}

	void Renderer::EndRenderPass()
	{
		s_RendererAPI->EndRenderPass();
	}

	void Renderer::BeginComputePass(const Ref<ComputePass>& computePass)
	{
		s_RendererAPI->BeginComputePass(computePass);
	}

	void Renderer::EndComputePass(const Ref<ComputePass>& computePass)
	{
		s_RendererAPI->EndComputePass(computePass);
	}

	void Renderer::DrawMesh(const Ref<Mesh>& mesh, uint32_t submeshIndex, const Ref<Material>& material)
	{
		s_RendererAPI->DrawMesh(mesh, submeshIndex, material);
	}

	void Renderer::DrawMesh(const Ref<Mesh>& mesh)
	{
		s_RendererAPI->DrawMesh(mesh);
	}

	void Renderer::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
		uint32_t indexCount)
	{
		s_RendererAPI->DrawIndexed(vertexBuffer, indexBuffer, material, indexCount);
	}

	void Renderer::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t indexCount)
	{
		s_RendererAPI->DrawIndexed(vertexBuffer, indexBuffer, indexCount);
	}

	void Renderer::DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount)
	{
		s_RendererAPI->DrawLines(vertexBuffer, vertexCount);
	}

	void Renderer::DrawFullScreenQuad()
	{
		s_RendererAPI->DrawFullScreenQuad();
		//Renderer::DrawIndexed(s_Data->FullscreenQuadVertexBuffer, s_Data->FullscreenQuadIndexBuffer);
	}

	std::pair<Ref<TextureCube>, Ref<TextureCube>> Renderer::CreateEnvironmentMap(const Ref<Texture2D>& equirectangularMap)
	{
		return s_RendererAPI->CreateEnvironmentMap(equirectangularMap);
	}

	RendererConfig& Renderer::GetConfig()
	{
		return s_Config;
	}

	void Renderer::SetConfig(const RendererConfig& config)
	{
		s_Config = config;
		RendererAPI::SetAPI(s_Config.APIType);
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

	Ref<Texture> Renderer::GetTextureInternal(const std::string& name)
	{
		if (s_Data->Textures.find(name) == s_Data->Textures.end())
			return nullptr;

		return s_Data->Textures[name];
	}
}