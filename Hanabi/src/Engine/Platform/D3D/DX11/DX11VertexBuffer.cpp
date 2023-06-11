#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11VertexBuffer.h"
#include "Engine/Platform/D3D/DXCommon.h"
#include "DX11Context.h"

namespace Hanabi
{
	DX11VertexBuffer::DX11VertexBuffer(uint32_t size)
	{
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.ByteWidth = size;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = m_Stride;
		DX_CHECK_RESULT(DX11Context::GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_VertexBuffer.GetAddressOf()));
	}

	DX11VertexBuffer::DX11VertexBuffer(const void* vertices, uint32_t size)
	{
		D3D11_SUBRESOURCE_DATA resourceData = {};
		resourceData.pSysMem = vertices;

		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.ByteWidth = size;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = m_Stride;
		DX_CHECK_RESULT(DX11Context::GetDevice()->CreateBuffer(&bufferDesc, &resourceData, m_VertexBuffer.GetAddressOf()));
	}

	DX11VertexBuffer::~DX11VertexBuffer()
	{
		m_VertexBuffer.Reset();
	}

	void DX11VertexBuffer::Bind() const
	{
		const UINT offset = 0;
		DX11Context::GetDeviceContext()->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
	}

	void DX11VertexBuffer::SetData(const void* data, uint32_t size)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		DX_CHECK_RESULT(DX11Context::GetDeviceContext()->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		memcpy(mappedResource.pData, data, size);
		DX11Context::GetDeviceContext()->Unmap(m_VertexBuffer.Get(), 0);
	}
}
#endif