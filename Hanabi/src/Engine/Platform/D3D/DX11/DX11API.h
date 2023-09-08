#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/RendererAPI.h"
#include "Engine/Renderer/RenderPass.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Platform/D3D/DXCommon.h"

#include <d3d11.h>

namespace Hanabi
{
	class DX11RendererAPI : public RendererAPI
	{
	public:
		void Init() override;
		void SetClearColor(const glm::vec4& color) override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear) override;
		void EndRenderPass(const Ref<RenderPass>& renderPass) override;

		void SubmitStaticMesh(const Ref<Mesh>& mesh, const Ref<Material>& material, const Ref<Pipeline>& pipeline) override;
		void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
			const Ref<Pipeline>& pipeline, uint32_t indexCount = 0) override;
		void DrawLines(const Ref<VertexBuffer>& vertexBuffer, const Ref<Material>& material, const Ref<Pipeline>& pipeline, uint32_t vertexCount) override;
	private:
		void SetBuffer(uint32_t width, uint32_t height, uint32_t x = 0, uint32_t y = 0);
		void Clear();

		uint32_t m_Width = 0, m_Height = 0;

		ComPtr<IDXGISwapChain> m_SwapChain;
		ComPtr<ID3D11Device> m_Device;
		ComPtr<ID3D11DeviceContext> m_DeviceContext;
		ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
		ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
		ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
		glm::vec4 m_ClearColor = { 0.0f,0.0f,0.0f,1.0f };
	};
}
#endif