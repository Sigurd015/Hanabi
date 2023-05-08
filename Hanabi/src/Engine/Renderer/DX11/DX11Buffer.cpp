#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "Engine/Renderer/DX11/DX11Buffer.h"
#include "Engine/Renderer/DX11/DX11Context.h"

namespace Hanabi
{
	void CreateBuffer(D3D11_BIND_FLAG bindFlag, uint32_t size, D3D11_USAGE usage, int cpuAccess, uint32_t stride,
		D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer)
	{
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.ByteWidth = size;
		bufferDesc.Usage = usage;
		bufferDesc.BindFlags = bindFlag;
		bufferDesc.CPUAccessFlags = cpuAccess;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = stride;
		DX11Context::GetDevice()->CreateBuffer(&bufferDesc, pInitialData, ppBuffer);
	}

	//-------------
	// VertexBuffer 
	//-------------

	DX11VertexBuffer::DX11VertexBuffer(uint32_t size)
	{
		CreateBuffer(D3D11_BIND_VERTEX_BUFFER, size, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, m_Stride, nullptr, m_VertexBuffer.GetAddressOf());
	}

	DX11VertexBuffer::DX11VertexBuffer(float* vertices, uint32_t size)
	{
		D3D11_SUBRESOURCE_DATA resourceData = {};
		resourceData.pSysMem = vertices;
		CreateBuffer(D3D11_BIND_VERTEX_BUFFER, size, D3D11_USAGE_DEFAULT, 0, m_Stride, &resourceData, m_VertexBuffer.GetAddressOf());
	}

	DX11VertexBuffer::~DX11VertexBuffer()
	{

	}

	void DX11VertexBuffer::Bind() const
	{
		const UINT offset = 0;
		DX11Context::GetDeviceContext()->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &offset);
	}

	void DX11VertexBuffer::Unbind() const
	{

	}

	void DX11VertexBuffer::SetData(const void* data, uint32_t size)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		DX11Context::GetDeviceContext()->Map(m_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, data, size);
		DX11Context::GetDeviceContext()->Unmap(m_VertexBuffer.Get(), 0);
	}

	//-------------
	// IndexBuffer 
	//-------------

	DX11IndexBuffer::DX11IndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count)
	{
		D3D11_SUBRESOURCE_DATA resourceData = {};
		resourceData.pSysMem = indices;
		CreateBuffer(D3D11_BIND_INDEX_BUFFER, count * sizeof(uint32_t), D3D11_USAGE_DEFAULT, 0, 0, &resourceData, m_IndexBuffer.GetAddressOf());
	}

	DX11IndexBuffer::~DX11IndexBuffer()
	{

	}

	void DX11IndexBuffer::Bind() const
	{
		DX11Context::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	void DX11IndexBuffer::Unbind() const
	{

	}
}
#endif