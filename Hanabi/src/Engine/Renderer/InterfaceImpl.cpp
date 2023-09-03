#include "hnbpch.h"

//----------OpenGL----------------------------------------
#include "Engine/Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Engine/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Engine/Platform/OpenGL/OpenGLAPI.h"
#include "Engine/Platform/OpenGL/OpenGLContext.h"
#include "Engine/Platform/OpenGL/OpenGLShader.h"
#include "Engine/Platform/OpenGL/OpenGLTexture.h"
#include "Engine/Platform/OpenGL/OpenGLFramebuffer.h"
#include "Engine/Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Engine/Platform/OpenGL/OpenGLPipeline.h"

#if defined(HNB_PLATFORM_WINDOWS)
//----------DX11----------------------------------------
#include "Engine/Platform/D3D/DX11/DX11VertexBuffer.h"
#include "Engine/Platform/D3D/DX11/DX11IndexBuffer.h"
#include "Engine/Platform/D3D/DX11/DX11API.h"
#include "Engine/Platform/D3D/DX11/DX11Context.h"
#include "Engine/Platform/D3D/DX11/DX11Shader.h"
#include "Engine/Platform/D3D/DX11/DX11Texture.h"
#include "Engine/Platform/D3D/DX11/DX11Pipeline.h"
#include "Engine/Platform/D3D/DX11/DX11Framebuffer.h"
#include "Engine/Platform/D3D/DX11/DX11ConstantBuffer.h"
#include "Engine/Platform/D3D/DX11/DX11RenderPass.h"
#include "Engine/Platform/D3D/DX11/DX11Image.h"
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
		case RendererAPIType::OpenGL:
			return CreateScope<OpenGLRendererAPI>();

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
		case RendererAPIType::OpenGL:
			HNB_CORE_ASSERT(false, "OpenGL RenderPass is currently not implemented!");
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
		case RendererAPIType::OpenGL:
			return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));

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
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLPipeline>(spec);

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
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLShader>(fileName);

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
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(size);

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
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(vertices, size);

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
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(indices, count);

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
		case RendererAPIType::OpenGL:
			HNB_CORE_ASSERT(false, "OpenGL Image2D is currently not implemented!");
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
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLTexture2D>(specification, data);

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
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLTextureCube>(specification, data);

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
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLFramebuffer>(spec);

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
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLUniformBuffer>(size);

			#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			return CreateRef<DX11ConstantBuffer>(size);
			#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}