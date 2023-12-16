#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11API.h"
#include "DX11Context.h"
#include "Hanabi/Core/Application.h"
#include "DX11RenderStates.h"
#include "Hanabi/Renderer/Renderer.h"
#include "DX11Texture.h"
#include "DX11ConstantBuffer.h"
#include "Hanabi/Asset/AssetManager/AssetManager.h"
#include "Hanabi/Renderer/ComputePass.h"
#include "Hanabi/Renderer/ComputePipeline.h"

#include <glm/gtc/type_ptr.hpp>

namespace Hanabi
{
	void DX11RendererAPI::Init()
	{
		m_DeviceContext = DX11Context::GetDeviceContext();
		m_Device = DX11Context::GetDevice();
		m_SwapChain = DX11Context::GetSwapChain();

		DX11RenderStates::Init();

		SetViewport(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void DX11RendererAPI::SetViewport(uint32_t width, uint32_t height)
	{
		m_RenderTargetView.Reset();
		m_DepthStencilBuffer.Reset();

		m_Width = width;
		m_Height = height;

		DX_CHECK_RESULT(m_SwapChain->ResizeBuffers(1, m_Width, m_Height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		DX_CHECK_RESULT(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));
		DX_CHECK_RESULT(m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_RenderTargetView.GetAddressOf()));

		// TODO: May need to support SRGB backbuffer
		//D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc = {};
		//targetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		//targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		//targetViewDesc.Texture2D.MipSlice = 0;
		//DX_CHECK_RESULT(m_Device->CreateRenderTargetView(backBuffer.Get(), &targetViewDesc, m_RenderTargetView.GetAddressOf()));
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
		m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);
	}

	void DX11RendererAPI::BeginComputePass(const Ref<ComputePass>& computePass)
	{
		Ref<ComputePipeline>& pipeline = computePass->GetPipeline();
		pipeline->Bind();

		computePass->BindInputs();
	}

	void DX11RendererAPI::EndComputePass(const Ref<ComputePass>& computePass)
	{
		computePass->UnbindInputs();
	}

	void DX11RendererAPI::DrawMesh(const Ref<Mesh>& mesh, uint32_t submeshIndex, const Ref<Material>& material)
	{
		material->Bind();

		Ref<MeshSource> source = mesh->GetMeshSource();
		source->GetVertexBuffer()->Bind();
		source->GetIndexBuffer()->Bind();

		HNB_CORE_ASSERT(submeshIndex <= source->GetSubmeshes().size(), "Submesh index out of range!");

		const Submesh& submesh = mesh->GetMeshSource()->GetSubmeshes()[submeshIndex];

		m_DeviceContext->DrawIndexed(submesh.IndexCount, submesh.BaseIndex, submesh.BaseVertex);
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

	void DX11RendererAPI::DrawFullScreenQuad()
	{
		m_DeviceContext->Draw(3, 0);
	}

	std::pair<Ref<TextureCube>, Ref<TextureCube>> DX11RendererAPI::CreateEnvironmentMap(const Ref<Texture2D>& equirectangularMap)
	{
		if (!Renderer::GetConfig().ComputeEnvironmentMaps)
			return { Renderer::GetTexture<TextureCube>("BlackCube"), Renderer::GetTexture<TextureCube>("BlackCube") };

		HNB_CORE_ASSERT(equirectangularMap->GetFormat() == ImageFormat::RGBA32F, "Texture is not HDR!");

		const uint32_t cubemapSize = Renderer::GetConfig().EnvironmentMapResolution;
		static const uint32_t irradianceMapSize = 32;

		Ref<DX11Texture2D> envMap = std::static_pointer_cast<DX11Texture2D>(equirectangularMap);

		TextureSpecification cubemapSpec = {};
		cubemapSpec.Width = cubemapSize;
		cubemapSpec.Height = cubemapSize;
		cubemapSpec.Format = ImageFormat::RGBA32F;

		Ref<DX11TextureCube> envUnfiltered = std::static_pointer_cast<DX11TextureCube>(TextureCube::Create(cubemapSpec));

		// Radiance map (Equirectangular to cubemap) 
		{
			static Ref<ComputePass> s_EquirectangularToCubemapPass = nullptr;
			if (!s_EquirectangularToCubemapPass)
			{
				ComputePassSpecification spec = {};
				ComputePipelineSpecification pipelineSpec = {};
				pipelineSpec.Shader = Renderer::GetShader("EquirectangularToCubemap");
				spec.Pipeline = ComputePipeline::Create(pipelineSpec);
				s_EquirectangularToCubemapPass = ComputePass::Create(spec);
			}

			envUnfiltered->CreateUAV();

			s_EquirectangularToCubemapPass->SetInput("u_EquirectangularMap", envMap);
			s_EquirectangularToCubemapPass->SetInput("o_OutputTex", envUnfiltered);

			BeginComputePass(s_EquirectangularToCubemapPass);
			s_EquirectangularToCubemapPass->Dispatch(cubemapSize / 32, cubemapSize / 32, 6);
			EndComputePass(s_EquirectangularToCubemapPass);

			envUnfiltered->GenerateMips();
		}

		// Radiance map (Filter)	
		Ref<DX11TextureCube> envFiltered = std::static_pointer_cast<DX11TextureCube>(TextureCube::Create(cubemapSpec));
		{
			// Copy Unfiltered envmap to Filtered envmap (Keep the first mip level)
			m_DeviceContext->CopyResource(envFiltered->GetTextureCube().Get(), envUnfiltered->GetTextureCube().Get());

			uint32_t mipCount = Utils::CalculateMipCount(cubemapSize, cubemapSize);

			struct CBFilterParam
			{
				float Roughness;

				char padding[12];
			};
			static Ref<ConstantBuffer> s_FilterParam;

			if (!s_FilterParam)
				s_FilterParam = ConstantBuffer::Create(sizeof(CBFilterParam));

			static Ref<ComputePass> s_EnvironmentMipFilterPass = nullptr;
			if (!s_EnvironmentMipFilterPass)
			{
				ComputePassSpecification spec = {};
				ComputePipelineSpecification pipelineSpec = {};
				pipelineSpec.Shader = Renderer::GetShader("EnvironmentMipFilter");
				spec.Pipeline = ComputePipeline::Create(pipelineSpec);
				s_EnvironmentMipFilterPass = ComputePass::Create(spec);
			}

			s_EnvironmentMipFilterPass->SetInput("u_InputTex", envUnfiltered);
			s_EnvironmentMipFilterPass->SetInput("CBFilterParam", s_FilterParam);

			const float deltaRoughness = 1.0f / glm::max((float)mipCount - 1.0f, 1.0f);
			for (uint32_t i = 1, size = cubemapSize; i < mipCount; i++, size /= 2)
			{
				uint32_t numGroups = glm::max(1u, size / 32);
				CBFilterParam filterParam = { i * deltaRoughness };
				s_FilterParam->SetData(&filterParam, sizeof(CBFilterParam));
				envFiltered->CreateUAV(i);
				s_EnvironmentMipFilterPass->SetInput("o_OutputTex", envFiltered);

				BeginComputePass(s_EnvironmentMipFilterPass);
				s_EnvironmentMipFilterPass->Dispatch(numGroups, numGroups, 6);
				EndComputePass(s_EnvironmentMipFilterPass);
			}
		}

		// Irradiance map
		cubemapSpec.Width = irradianceMapSize;
		cubemapSpec.Height = irradianceMapSize;
		Ref<DX11TextureCube> irradianceMap = std::static_pointer_cast<DX11TextureCube>(TextureCube::Create(cubemapSpec));
		{
			struct CBSamplesParams
			{
				uint32_t Samples;

				char padding[12];
			};
			static Ref<ConstantBuffer> s_SamplesParam;

			if (!s_SamplesParam)
				s_SamplesParam = ConstantBuffer::Create(sizeof(CBSamplesParams));

			static Ref<ComputePass> s_EnvironmentIrradiancePass = nullptr;
			if (!s_EnvironmentIrradiancePass)
			{
				ComputePassSpecification spec = {};
				ComputePipelineSpecification pipelineSpec = {};
				pipelineSpec.Shader = Renderer::GetShader("EnvironmentIrradiance");
				spec.Pipeline = ComputePipeline::Create(pipelineSpec);
				s_EnvironmentIrradiancePass = ComputePass::Create(spec);
			}

			irradianceMap->CreateUAV();

			s_EnvironmentIrradiancePass->SetInput("u_RadianceMap", envFiltered);
			s_EnvironmentIrradiancePass->SetInput("o_IrradianceMap", irradianceMap);
			s_EnvironmentIrradiancePass->SetInput("CBSamplesParams", s_SamplesParam);

			CBSamplesParams samplesParams = { Renderer::GetConfig().IrradianceMapComputeSamples };
			s_SamplesParam->SetData(&samplesParams, sizeof(CBSamplesParams));

			BeginComputePass(s_EnvironmentIrradiancePass);
			s_EnvironmentIrradiancePass->Dispatch(irradianceMapSize / 32, irradianceMapSize / 32, 6);
			EndComputePass(s_EnvironmentIrradiancePass);

			irradianceMap->GenerateMips();
		}

		return { envFiltered, irradianceMap };
	}
}
#endif