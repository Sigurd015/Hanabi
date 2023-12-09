#pragma once
#include "Camera.h"
#include "Shader.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Mesh.h"
#include "MaterialAsset.h"
#include "RendererConfig.h"

namespace Hanabi
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear = true);
		static void EndRenderPass();

		//3D Objects
		static void DrawMesh(const Ref<Mesh>& mesh, uint32_t submeshIndex, const Ref<Material>& material);
		static void DrawMesh(const Ref<Mesh>& mesh);

		//2D Objects
		static void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
			uint32_t indexCount);
		static void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t indexCount = 0);
		static void DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount);

		static void DrawFullScreenQuad();

		static std::pair<Ref<TextureCube>, Ref<TextureCube>> Renderer::CreateEnvironmentMap(const Ref<Texture2D>& equirectangularMap);

		static RendererConfig& GetConfig();
		static void SetConfig(const RendererConfig& config);

		static Ref<Shader> GetShader(const std::string& name);
		static Ref<Mesh> GetMesh(const std::string& name);

		template<typename T>
		static Ref<T> GetTexture(const std::string& name)
		{
			static_assert(std::is_base_of<Texture, T>::value, "Renderer::GetTexture only works for types derived from Texture");

			Ref<Texture> texture = GetTextureInternal(name);
			return std::static_pointer_cast<T>(texture);
		}
	private:
		static Ref<Texture> GetTextureInternal(const std::string& name);
	};
}
