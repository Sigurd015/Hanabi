#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11API.h"
#include "Engine/Platform/D3D/DXCommon.h"
#include "DX11Context.h"
#include "Engine/Core/Application.h"

namespace Hanabi
{
	static D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopologyTypeToD3D(PrimitiveTopology type)
	{
		switch (type)
		{
		case PrimitiveTopology::Points:
			return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case PrimitiveTopology::Lines:
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case PrimitiveTopology::Triangles:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}

		HNB_CORE_ASSERT(false, "Unknown Primitive Topology!");
	}

	void DX11RendererAPI::Init()
	{
		m_DeviceContext = DX11Context::GetDeviceContext();
		m_Device = DX11Context::GetDevice();
		m_SwapChain = DX11Context::GetSwapChain();

		SetBuffer(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void DX11RendererAPI::SetClearColor(const glm::vec4& color)
	{
		m_ClearColor = color;
	}

	void DX11RendererAPI::ResetToBackBuffer()
	{
		m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), &m_ClearColor.x);
		m_DeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());
	}

	void DX11RendererAPI::SetBuffer(uint32_t width, uint32_t height, uint32_t x, uint32_t y)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		DX_CHECK_RESULT(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));
		DX_CHECK_RESULT(m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_RenderTargetView.GetAddressOf()));

		D3D11_TEXTURE2D_DESC depthStencilDesc = {};
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DX_CHECK_RESULT(m_Device->CreateTexture2D(&depthStencilDesc, nullptr, m_DepthStencilBuffer.GetAddressOf()));
		DX_CHECK_RESULT(m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), nullptr, m_DepthStencilView.GetAddressOf()));
		m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());

		D3D11_VIEWPORT viewPort{};
		viewPort.Width = width;
		viewPort.Height = height;
		viewPort.MinDepth = 0;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = x;
		viewPort.TopLeftY = y;
		m_DeviceContext->RSSetViewports(1, &viewPort);
	}

	void DX11RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		m_RenderTargetView.Reset();
		m_DepthStencilView.Reset();
		m_DepthStencilBuffer.Reset();

		DX_CHECK_RESULT(m_SwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

		SetBuffer(width, height, x, y);
	}

	void DX11RendererAPI::BeginRender()
	{
		ResetToBackBuffer();
	}

	void DX11RendererAPI::BeginRenderPass(const Ref<RenderPass>& renderPass)
	{
		renderPass->GetSpecification().TargetFramebuffer->ClearAttachment();
		renderPass->GetSpecification().TargetFramebuffer->Bind();
	}

	void DX11RendererAPI::EndRenderPass(const Ref<RenderPass>& renderPass)
	{
		renderPass->GetSpecification().TargetFramebuffer->Unbind();
	}

	void DX11RendererAPI::EndRender()
	{
		ResetToBackBuffer();
	}

	void DX11RendererAPI::SubmitStaticMesh(const Ref<Mesh>& mesh, const  Ref<Pipeline>& pipeline)
	{
		mesh->GetVertexBuffer()->Bind();
		mesh->GetIndexBuffer()->Bind();
		pipeline->Bind();
		mesh->GetMaterial()->Bind();

		m_DeviceContext->IASetPrimitiveTopology(PrimitiveTopologyTypeToD3D(pipeline->GetSpecification().Topology));
		uint32_t count = mesh->GetIndexBuffer()->GetCount();
		m_DeviceContext->DrawIndexed(count, 0, 0);
	}

	void DX11RendererAPI::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer,
		const Ref<Pipeline>& pipeline, uint32_t indexCount)
	{
		vertexBuffer->Bind();
		indexBuffer->Bind();
		pipeline->Bind();
		pipeline->GetSpecification().Shader->Bind();

		m_DeviceContext->IASetPrimitiveTopology(PrimitiveTopologyTypeToD3D(pipeline->GetSpecification().Topology));
		uint32_t count = indexCount ? indexCount : indexBuffer->GetCount();
		m_DeviceContext->DrawIndexed(count, 0, 0);
	}

	void DX11RendererAPI::DrawLines(const Ref<VertexBuffer>& vertexBuffer, const Ref<Pipeline>& pipeline, uint32_t vertexCount)
	{
		vertexBuffer->Bind();
		pipeline->Bind();
		pipeline->GetSpecification().Shader->Bind();

		m_DeviceContext->IASetPrimitiveTopology(PrimitiveTopologyTypeToD3D(pipeline->GetSpecification().Topology));
		m_DeviceContext->Draw(vertexCount, 0);
	}

	void DX11RendererAPI::SetLineWidth(float width)
	{}
}
#endif