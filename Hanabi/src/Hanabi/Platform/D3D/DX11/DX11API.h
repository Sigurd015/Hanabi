#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Hanabi/Renderer/RendererAPI.h"
#include "Hanabi/Renderer/RenderPass.h"
#include "Hanabi/Renderer/Shader.h"
#include "Hanabi/Platform/D3D/DXCommon.h"

#include <d3d11.h>

namespace Hanabi
{
	class DX11RendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t width, uint32_t height) override;

		virtual void BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear) override;
		virtual void EndRenderPass() override;

		virtual void DrawMesh(const Ref<Mesh>& mesh, uint32_t submeshIndex, const Ref<Material>& material) override;
		virtual void DrawMesh(const Ref<Mesh>& mesh) override;

		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
			uint32_t indexCount = 0) override;
		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t indexCount = 0) override;
		virtual void DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount) override;
		virtual void DrawFullScreenQuad() override;

		virtual std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const Ref<Texture2D>& equirectangularMap) override;
	private:
		uint32_t m_Width = 0, m_Height = 0;

		ComPtr<IDXGISwapChain> m_SwapChain;
		ComPtr<ID3D11Device> m_Device;
		ComPtr<ID3D11DeviceContext> m_DeviceContext;
		ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
		ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
	};
}
#endif