#include "hnbpch.h"
#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11IndexBuffer.h"
#include "DX11Context.h"

namespace Hanabi
{
	DX11IndexBuffer::DX11IndexBuffer(void* indices, uint32_t count) : m_Count(count)
	{
		D3D11_SUBRESOURCE_DATA resourceData = {};
		resourceData.pSysMem = indices;

		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.ByteWidth = count * sizeof(uint32_t);
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		DX_CHECK_RESULT(DX11Context::GetDevice()->CreateBuffer(&bufferDesc, &resourceData, m_IndexBuffer.GetAddressOf()));
	}

	DX11IndexBuffer::~DX11IndexBuffer()
	{
		m_IndexBuffer.Reset();
	}

	void DX11IndexBuffer::Bind() const
	{
		DX11Context::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
}
#endif