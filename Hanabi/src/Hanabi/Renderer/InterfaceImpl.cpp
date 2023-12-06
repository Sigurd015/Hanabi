#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
//----------DX11----------------------------------------
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

			#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			return CreateScope<DX11RendererAPI>();
			#endif
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

			#if defined(HNB_PLATFORM_WINDOWS)
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

			#if defined(HNB_PLATFORM_WINDOWS)
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

			#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			return CreateRef<DX11Pipeline>(spec);
			#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& fileName)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

			#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			return CreateRef<DX11Shader>(fileName);
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

			#if defined(HNB_PLATFORM_WINDOWS)
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

			#if defined(HNB_PLATFORM_WINDOWS)
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

			#if defined(HNB_PLATFORM_WINDOWS)
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

			#if defined(HNB_PLATFORM_WINDOWS)
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

			#if defined(HNB_PLATFORM_WINDOWS)
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

			#if defined(HNB_PLATFORM_WINDOWS)
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

			#if defined(HNB_PLATFORM_WINDOWS)
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

			#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			return CreateRef<DX11ConstantBuffer>(size);
			#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}