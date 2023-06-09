#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11ConstantBuffer.h"
#include "Engine/Platform/D3D/DXCommon.h"
#include "DX11Context.h"

namespace Hanabi
{
	DX11ConstantBuffer::DX11ConstantBuffer(uint32_t size, uint32_t binding) :m_BindingID(binding), m_DataSize(size)
	{
		D3D11_BUFFER_DESC buffer = {};
		buffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer.Usage = D3D11_USAGE_DYNAMIC;
		buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer.MiscFlags = 0;
		buffer.ByteWidth = m_DataSize;
		buffer.StructureByteStride = 0;
		DX_CHECK_RESULT(DX11Context::GetDevice()->CreateBuffer(&buffer, nullptr, m_ConstantBuffer.GetAddressOf()));
	}

	DX11ConstantBuffer::~DX11ConstantBuffer()
	{
		m_ConstantBuffer.Reset();
	}

	void DX11ConstantBuffer::Bind() const
	{
		DX11Context::GetDeviceContext()->VSSetConstantBuffers(m_BindingID, 1, m_ConstantBuffer.GetAddressOf());
		DX11Context::GetDeviceContext()->PSSetConstantBuffers(m_BindingID, 1, m_ConstantBuffer.GetAddressOf());
	}

	void DX11ConstantBuffer::SetData(const void* data, uint32_t offset)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		DX_CHECK_RESULT(DX11Context::GetDeviceContext()->Map(m_ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
		memcpy(mappedResource.pData, data, m_DataSize);
		DX11Context::GetDeviceContext()->Unmap(m_ConstantBuffer.Get(), 0);
	}
}
#endif