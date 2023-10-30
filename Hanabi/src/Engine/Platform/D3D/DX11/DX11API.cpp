#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11API.h"
#include "DX11Context.h"
#include "Engine/Core/Application.h"
#include "DX11RenderStates.h"
#include "Engine/Renderer/Renderer.h"
#include "DX11Texture.h"
#include "DX11ConstantBuffer.h"

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
		m_DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
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

	std::pair<Ref<TextureCube>, Ref<TextureCube>> DX11RendererAPI::CreateEnvironmentMap(const Ref<Texture2D>& equirectangularMap)
	{
		if (!Renderer::GetConfig().ComputeEnvironmentMaps)
			return { Renderer::GetTexture<TextureCube>("BlackCube"), Renderer::GetTexture<TextureCube>("BlackCube") };

		HNB_CORE_ASSERT(equirectangularMap->GetFormat() == ImageFormat::RGBA32F, "Texture is not HDR!");

		const uint32_t cubemapSize = Renderer::GetConfig().EnvironmentMapResolution;
		static const uint32_t irradianceMapSize = 32;
		static ID3D11UnorderedAccessView* nullUAV[] = { nullptr };

		TextureSpecification cubemapSpec = {};
		cubemapSpec.Width = cubemapSize;
		cubemapSpec.Height = cubemapSize;
		cubemapSpec.Format = ImageFormat::RGBA32F;

		Ref<DX11TextureCube> envUnfiltered = std::static_pointer_cast<DX11TextureCube>(TextureCube::Create(cubemapSpec));
		Ref<DX11TextureCube> envFiltered = std::static_pointer_cast<DX11TextureCube>(TextureCube::Create(cubemapSpec));

		Ref<DX11Texture2D> envMap = std::static_pointer_cast<DX11Texture2D>(equirectangularMap);

		// Radiance map (Equirectangular to cubemap) 
		{
			Ref<Shader> equirectangularToCubemapShader = Renderer::GetShader("EquirectangularToCubemap");

			const ShaderReflectionData& reflectionData = equirectangularToCubemapShader->GetReflectionData();
			uint32_t uavSlot = 0;
			{
				auto it = reflectionData.find("o_OutputTex");
				if (it != reflectionData.end())
				{
					uavSlot = it->second;
					m_DeviceContext->CSSetUnorderedAccessViews(uavSlot, 1, envUnfiltered->GetUAV().GetAddressOf(), 0);
				}
				else
				{
					HNB_CORE_WARN("RendererResource o_OutputTex not found in shader!");
				}
			}
			{
				auto it = reflectionData.find("u_EquirectangularMap");
				if (it != reflectionData.end())
				{
					m_DeviceContext->CSSetShaderResources(it->second, 1, envMap->GetTextureSRV().GetAddressOf());
				}
				else
				{
					HNB_CORE_WARN("RendererResource u_EquirectangularMap not found in shader!");
				}
			}
			{
				auto it = reflectionData.find("u_SSLinearWrap");
				if (it != reflectionData.end())
				{
					m_DeviceContext->CSSetSamplers(it->second, 1, DX11RenderStates::SSLinearWrap.GetAddressOf());
				}
			}

			equirectangularToCubemapShader->Bind();
			m_DeviceContext->Dispatch(cubemapSize / 32, cubemapSize / 32, 6);

			// Unbind uav
			m_DeviceContext->CSSetUnorderedAccessViews(uavSlot, 1, nullUAV, nullptr);

			envUnfiltered->GenerateMips();
		}

		// Radiance map (Filter)
		{
			Ref<Shader> environmentMipFilterShader = Renderer::GetShader("EnvironmentMipFilter");
			uint32_t mipCount = Utils::CalculateMipCount(cubemapSize, cubemapSize);
			const ShaderReflectionData& reflectionData = environmentMipFilterShader->GetReflectionData();
			{
				auto it = reflectionData.find("u_InputTex");
				if (it != reflectionData.end())
				{
					m_DeviceContext->CSSetShaderResources(it->second, 1, envUnfiltered->GetTextureSRV().GetAddressOf());
				}
				else
				{
					HNB_CORE_WARN("RendererResource u_InputTex not found in shader!");
				}
			}
			{
				auto it = reflectionData.find("u_SSLinearWrap");
				if (it != reflectionData.end())
				{
					m_DeviceContext->CSSetSamplers(it->second, 1, DX11RenderStates::SSLinearWrap.GetAddressOf());
				}
			}
			uint32_t uavSlot = 0;
			{
				auto it = reflectionData.find("o_OutputTex");
				if (it != reflectionData.end())
				{
					uavSlot = it->second;
				}
				else
				{
					HNB_CORE_WARN("RendererResource o_OutputTex not found in shader!");
				}
			}

			struct CBFilterParam
			{
				float Roughness;

				char padding[12];
			};

			static Ref<DX11ConstantBuffer> s_FilterParam;

			if (!s_FilterParam)
				s_FilterParam = std::static_pointer_cast<DX11ConstantBuffer>(ConstantBuffer::Create(sizeof(CBFilterParam)));

			{
				auto it = reflectionData.find("CBFilterParam");
				if (it != reflectionData.end())
				{
					m_DeviceContext->CSSetConstantBuffers(it->second, 1, s_FilterParam->GetBuffer().GetAddressOf());
				}
				else
				{
					HNB_CORE_WARN("RendererResource CBFilterParam not found in shader!");
				}
			}

			environmentMipFilterShader->Bind();
			const float deltaRoughness = 1.0f / glm::max((float)mipCount - 1.0f, 1.0f);
			for (uint32_t i = 0, size = cubemapSize; i < mipCount; i++, size /= 2)
			{
				uint32_t numGroups = glm::max(1u, size / 32);
				float roughness = i * deltaRoughness;
				roughness = glm::max(roughness, 0.05f);

				CBFilterParam filterParam = { roughness };
				s_FilterParam->SetData(&filterParam, sizeof(CBFilterParam));

				m_DeviceContext->CSSetUnorderedAccessViews(uavSlot, 1, envFiltered->GetUAV().GetAddressOf(), 0);
				m_DeviceContext->Dispatch(numGroups, numGroups, 6);
			}

			// Unbind uav
			m_DeviceContext->CSSetUnorderedAccessViews(uavSlot, 1, nullUAV, nullptr);
		}

		// Irradiance map
		{
			cubemapSpec.Width = irradianceMapSize;
			cubemapSpec.Height = irradianceMapSize;
			Ref<TextureCube> irradianceMap = TextureCube::Create(cubemapSpec);

			// TODO: Implement this
		}

		return { envFiltered, Renderer::GetTexture<TextureCube>("BlackCube") };
	}
}
#endif