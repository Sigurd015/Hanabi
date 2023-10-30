#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/ConstantBuffer.h"
#include "Engine/Platform/D3D/DXCommon.h"

#include <d3d11.h>

namespace Hanabi
{
	class DX11ConstantBuffer : public ConstantBuffer
	{
	public:
		DX11ConstantBuffer(uint32_t size);
		~DX11ConstantBuffer();
		void Bind(uint32_t slot) const override;
		void SetData(const void* data, uint32_t offset = 0) override;

		// D3D Interop API
		ComPtr<ID3D11Buffer> GetBuffer() const { return m_ConstantBuffer; }
	private:
		uint32_t m_DataSize;
		ComPtr<ID3D11Buffer> m_ConstantBuffer;
	};
}
#endif