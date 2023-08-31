#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/ConstantBuffer.h"

#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>

namespace Hanabi
{
	class DX11ConstantBuffer : public ConstantBuffer
	{
	public:
		DX11ConstantBuffer(uint32_t size);
		~DX11ConstantBuffer();
		void Bind(uint32_t binding) const override;
		void SetData(const void* data, uint32_t offset = 0) override;
	private:
		uint32_t m_DataSize;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_ConstantBuffer;
	};
}
#endif