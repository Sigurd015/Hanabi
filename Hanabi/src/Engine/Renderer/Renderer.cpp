#include "hnbpch.h"
#include "RendererAPI.h"
#include "Renderer2D.h"
#include "Renderer.h"
#include "MeshFactory.h"
#include "SceneRenderer.h"
#include "Engine/Asset/AssetSerializer/AssetSerializer.h"

namespace Hanabi
{
	static Scope<RendererAPI> s_RendererAPI = nullptr;

	static RendererConfig s_Config;

	struct RendererData
	{
		Ref<ShaderLibrary> ShaderLibrary;

		std::unordered_map<std::string, Ref<Texture>> Textures;
		std::unordered_map<std::string, Ref<Mesh>> Meshes;
		Ref<VertexBuffer> FullscreenQuadVertexBuffer;
		Ref<IndexBuffer> FullscreenQuadIndexBuffer;
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

		//Setup textures
		TextureSpecification spec;
		spec.Format = ImageFormat::RGBA8;
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
		s_Data->Textures["BRDFLut"] = TextureSerializer::LoadTexture2D("Resources/Renderer/BRDF_LUT.tga", spec);

		//Load default meshes
		s_Data->Meshes["Box"] = MeshFactory::CreateBox({ 1.0f,1.0f,1.0f });
		s_Data->Meshes["Capsule"] = MeshFactory::CreateCapsule(1.0f, 1.0f);
		s_Data->Meshes["Sphere"] = MeshFactory::CreateSphere(1.0f);

		// Create fullscreen quad
		{
			float x = -1;
			float y = -1;
			float width = 2, height = 2;
			struct QuadVertex
			{
				glm::vec3 Position;
				glm::vec2 TexCoord;
			};

			QuadVertex* data = new QuadVertex[4];

			data[0].Position = glm::vec3(x, y, 0.0f);
			data[0].TexCoord = glm::vec2(0, 0);

			data[1].Position = glm::vec3(x + width, y, 0.0f);
			data[1].TexCoord = glm::vec2(1, 0);

			data[2].Position = glm::vec3(x + width, y + height, 0.0f);
			data[2].TexCoord = glm::vec2(1, 1);

			data[3].Position = glm::vec3(x, y + height, 0.0f);
			data[3].TexCoord = glm::vec2(0, 1);

			s_Data->FullscreenQuadVertexBuffer = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
			s_Data->FullscreenQuadVertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				});

			uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
			s_Data->FullscreenQuadIndexBuffer = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));

			delete[] data;
		}

		Renderer2D::Init();
		SceneRenderer::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		SceneRenderer::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(0, 0, width, height);
	}

	void Renderer::BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear)
	{
		s_RendererAPI->BeginRenderPass(renderPass, clear);
	}

	void Renderer::EndRenderPass()
	{
		s_RendererAPI->EndRenderPass();
	}

	void Renderer::DrawMesh(const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		s_RendererAPI->DrawMesh(mesh, material);
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
		Renderer::DrawIndexed(s_Data->FullscreenQuadVertexBuffer, s_Data->FullscreenQuadIndexBuffer);
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