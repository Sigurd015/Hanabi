#include "hnbpch.h"
#include "Engine/Renderer/Buffer.h"
#include "Engine/Renderer/RendererAPI.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/RenderingContext.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Framebuffer.h"
#include "Engine/Renderer/UniformBuffer.h"
//----------OpenGL----------------------------------------
#include "Engine/Platform/OpenGL/OpenGLBuffer.h"
#include "Engine/Platform/OpenGL/OpenGLAPI.h"
#include "Engine/Platform/OpenGL/OpenGLContext.h"
#include "Engine/Platform/OpenGL/OpenGLShader.h"
#include "Engine/Platform/OpenGL/OpenGLTexture.h"
#include "Engine/Platform/OpenGL/OpenGLVertexArray.h"
#include "Engine/Platform/OpenGL/OpenGLFramebuffer.h"
#include "Engine/Platform/OpenGL/OpenGLUniformBuffer.h"

#if defined(HNB_PLATFORM_WINDOWS)
//----------DX11----------------------------------------
#include "Engine/Platform/D3D/DX11/DX11Buffer.h"
#include "Engine/Platform/D3D/DX11/DX11API.h"
#include "Engine/Platform/D3D/DX11/DX11Context.h"
#include "Engine/Platform/D3D/DX11/DX11Shader.h"
#include "Engine/Platform/D3D/DX11/DX11Texture.h"
#include "Engine/Platform/D3D/DX11/DX11VertexDeclaration.h"
#include "Engine/Platform/D3D/DX11/DX11Framebuffer.h"
#include "Engine/Platform/D3D/DX11/DX11ConstantBuffer.h"
#endif

namespace Hanabi
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = nullptr;

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

	Scope<RenderingContext> RenderingContext::Create(void* window)
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

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLShader>(filepath);

#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			return CreateScope<DX11Shader>(filepath);
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexArray> VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPIType::None:
			HNB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPIType::OpenGL:
			return CreateRef<OpenGLVertexArray>();

#if defined(HNB_PLATFORM_WINDOWS)
		case RendererAPIType::DX11:
			return CreateScope<DX11VertexDeclaration>();
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
			return CreateScope<DX11VertexBuffer>(size);
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
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
			return CreateScope<DX11VertexBuffer>(vertices, size);
#endif
		}
		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
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
			return CreateScope<DX11IndexBuffer>(indices, count);
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
			return CreateScope<DX11Texture2D>(path);
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
			return CreateScope<DX11Texture2D>(specification);
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
			return CreateScope<DX11Framebuffer>(spec);
#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
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
			return CreateScope<DX11ConstantBuffer>(size, binding);
#endif
		}

		HNB_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}