#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/Buffer.h"

#include <d3d11.h>
#include <wrl.h>

namespace Hanabi
{
	class DX11VertexBuffer : public VertexBuffer
	{
	public:
		DX11VertexBuffer(uint32_t size);
		DX11VertexBuffer(float* vertices, uint32_t size);
		~DX11VertexBuffer();
		void Bind() const override;
		void Unbind() const override;
		void SetData(const void* data, uint32_t size) override;
		const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout)override { m_Layout = layout;	m_Stride = m_Layout.GetStride(); }
	private:
		uint32_t m_Stride = 0;
		BufferLayout m_Layout;
		static uint32_t m_RendererID;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_VertexBuffer;
	};

	class DX11IndexBuffer : public IndexBuffer
	{
	public:
		DX11IndexBuffer(uint32_t* indices, uint32_t count);
		~DX11IndexBuffer();
		void Bind() const override;
		void Unbind() const override;
		uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_Count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
	};
}
#endif