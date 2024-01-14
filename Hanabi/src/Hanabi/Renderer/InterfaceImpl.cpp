#include "hnbpch.h"

//----------Vulkan----------------------------------------
#ifdef HNB_ENABLE_VULKAN
#include "Hanabi/Platform/Vulkan/VulkanContext.h"
#endif

//----------DirectX----------------------------------------
#ifdef HNB_ENABLE_D3D
//----------DX11----------------------------------------
#include "Hanabi/Platform/D3D/DXShaderCompiler.h"
#include "Hanabi/Platform/D3D/DX11/DX11VertexBuffer.h"
#include "Hanabi/Platform/D3D/DX11/DX11IndexBuffer.h"
#include "Hanabi/Platform/D3D/DX11/DX11API.h"
#include "Hanabi/Platform/D3D/DX11/DX11Context.h"
#include "Hanabi/Platform/D3D/DX11/DX11Shader.h"
#include "Hanabi/Platform/D3D/DX11/DX11Texture.h"
#include "Hanabi/Platform/D3D/DX11/DX11Pipeline.h"
#include "Hanabi/Platform/D3D/DX11/DX11Framebuffer.h"
#include "Hanabi/Platform/D3D/DX11/DX11ConstantBuffer.h"
#include "Hanabi/Platform/D3D/DX11/DX11RenderPass.h"
#include "Hanabi/Platform/D3D/DX11/DX11Image.h"
#include "Hanabi/Platform/D3D/DX11/DX11ComputePass.h"
#include "Hanabi/Platform/D3D/DX11/DX11ComputePipeline.h"
#endif

namespace Hanabi
{
	RendererAPIType RendererAPI::s_API = RendererAPIType::None;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateScope<DX11RendererAPI>();
#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Scope<ShaderCompiler> ShaderCompiler::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateScope<DXShaderCompiler>();
#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ComputePass> ComputePass::Create(const ComputePassSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPIType::DX11:
			return CreateRef<DX11ComputePass>(spec);
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ComputePipeline> ComputePipeline::Create(const ComputePipelineSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPIType::DX11:
			return CreateRef<DX11ComputePipeline>(spec);
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateRef<DX11RenderPass>(spec);
#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Scope<RendererContext> RendererContext::Create(void* window)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_VULKAN
		case RendererAPIType::Vulkan:
			return CreateScope<VulkanContext>(static_cast<GLFWwindow*>(window));
#endif
#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateScope<DX11Context>(static_cast<HWND*>(window));
#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateRef<DX11Pipeline>(spec);
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateRef<DX11VertexBuffer>(size);
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateRef<DX11VertexBuffer>(vertices, size);
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(void* indices, uint32_t count)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateRef<DX11IndexBuffer>(indices, count);
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Image2D> Image2D::Create(const ImageSpecification& specification, Buffer buffer)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateRef<DX11Image2D>(specification, buffer);
#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification, Buffer data)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateRef<DX11Texture2D>(specification, data);
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(const TextureSpecification& specification, Buffer data)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateRef<DX11TextureCube>(specification, data);
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateRef<DX11Framebuffer>(spec);
#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ConstantBuffer> ConstantBuffer::Create(uint32_t size)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

#ifdef HNB_ENABLE_D3D
		case RendererAPIType::DX11:
			return CreateRef<DX11ConstantBuffer>(size);
#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}