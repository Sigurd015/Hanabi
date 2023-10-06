#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11API.h"
#include "DX11Context.h"
#include "Engine/Core/Application.h"
#include "DX11RenderStates.h"

#include <glm/gtc/type_ptr.hpp>

namespace Hanabi
{
	void DX11RendererAPI::Init()
	{
		m_DeviceContext = DX11Context::GetDeviceContext();
		m_Device = DX11Context::GetDevice();
		m_SwapChain = DX11Context::GetSwapChain();

		DX11RenderStates::Init();

		SetBuffer(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void DX11RendererAPI::Clear()
	{
		D3D11_VIEWPORT viewPort{};
		viewPort.Width = m_Width;
		viewPort.Height = m_Height;
		viewPort.MinDepth = 0;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
		m_DeviceContext->RSSetViewports(1, &viewPort);

		static const glm::vec4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), glm::value_ptr(clearColor));
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

		m_Width = width;
		m_Height = height;
	}

	void DX11RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		m_RenderTargetView.Reset();
		m_DepthStencilView.Reset();
		m_DepthStencilBuffer.Reset();

		DX_CHECK_RESULT(m_SwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

		SetBuffer(width, height, x, y);
	}

	void DX11RendererAPI::BeginRenderPass(const Ref<RenderPass>& renderPass, bool clear)
	{
		if (clear)
		{
			renderPass->GetTargetFramebuffer()->ClearAttachment();
		}

		renderPass->GetTargetFramebuffer()->Bind();

		Ref<Pipeline>& pipeline = renderPass->GetPipeline();
		pipeline->Bind();

		PipelineSpecification& spec = pipeline->GetSpecification();

		if (!spec.DepthTest)
			m_DeviceContext->OMSetDepthStencilState(DX11RenderStates::DSSNoDepthTest.Get(), 0);
		else
		{
			switch (spec.DepthOperator)
			{
			case DepthCompareOperator::Less:
				m_DeviceContext->OMSetDepthStencilState(DX11RenderStates::DSSLess.Get(), 0);
				break;
			case DepthCompareOperator::LessEqual:
				m_DeviceContext->OMSetDepthStencilState(DX11RenderStates::DSSLessEqual.Get(), 0);
				break;
			}
		}

		if (spec.BackfaceCulling)
			m_DeviceContext->RSSetState(DX11RenderStates::RSCullBack.Get());
		else
			m_DeviceContext->RSSetState(DX11RenderStates::RSNoCull.Get());

		D3D11_VIEWPORT viewPort{};
		viewPort.MinDepth = 0;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;

		if (spec.TargetFramebuffer->GetSpecification().SwapChainTarget)
		{
			viewPort.Width = m_Width;
			viewPort.Height = m_Height;
		}
		else
		{
			viewPort.Width = spec.TargetFramebuffer->GetWidth();
			viewPort.Height = spec.TargetFramebuffer->GetHeight();
		}

		m_DeviceContext->RSSetViewports(1, &viewPort);

		renderPass->BindInputs();
	}

	void DX11RendererAPI::EndRenderPass()
	{
		DX11Context::GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
		Clear();
	}

	void DX11RendererAPI::DrawMesh(const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		material->Bind();
		DrawMesh(mesh);
	}

	void DX11RendererAPI::DrawMesh(const Ref<Mesh>& mesh)
	{
		Ref<MeshSource> source = mesh->GetMeshSource();
		source->GetVertexBuffer()->Bind();
		source->GetIndexBuffer()->Bind();

		m_DeviceContext->DrawIndexed(source->GetIndexBuffer()->GetCount(), 0, 0);
	}

	void DX11RendererAPI::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, const Ref<Material>& material,
		uint32_t indexCount)
	{
		material->Bind();

		DrawIndexed(vertexBuffer, indexBuffer, indexCount);
	}

	void DX11RendererAPI::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t indexCount)
	{
		vertexBuffer->Bind();
		indexBuffer->Bind();

		if (indexCount == 0)
			m_DeviceContext->DrawIndexed(indexBuffer->GetCount(), 0, 0);
		else
			m_DeviceContext->DrawIndexed(indexCount, 0, 0);
	}

	void DX11RendererAPI::DrawLines(const Ref<VertexBuffer>& vertexBuffer, uint32_t vertexCount)
	{
		vertexBuffer->Bind();

		m_DeviceContext->Draw(vertexCount, 0);
	}
}
#endif