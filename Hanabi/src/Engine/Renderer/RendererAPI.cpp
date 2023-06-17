#include "hnbpch.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RendererAPI.h"
#include "RenderPass.h"
#include "RendererContext.h"
#include "Shader.h"
#include "Texture.h"
#include "Pipeline.h"
#include "Framebuffer.h"
#include "ConstantBuffer.h"
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
#endif

namespace Hanabi
{
	RendererAPIType RendererAPI::s_API = RendererAPIType::None;

	Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		return CreateRef<RenderPass>(spec);
	}

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

	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			return nullptr;
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLPipeline>(spec);
		case RendererAPIType::DX11:
			return CreateRef<DX11Pipeline>(spec);
		}
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

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPIType::OpenGL:
			return  CreateRef<OpenGLTexture2D>(path);

#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			return CreateRef<DX11Texture2D>(path);
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLTexture2D>(specification);

#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			return CreateRef<DX11Texture2D>(specification);
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

	Ref<ConstantBuffer> ConstantBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLUniformBuffer>(size, binding);

#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			return CreateRef<DX11ConstantBuffer>(size, binding);
#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}