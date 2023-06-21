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
		DX11ConstantBuffer(uint32_t size, uint32_t binding);
		~DX11ConstantBuffer();
		void Bind() const override;
		void SetData(const void* data, uint32_t offset = 0) override;
	private:
		uint32_t m_DataSize;
		uint32_t m_BindingID;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_ConstantBuffer;
	};
}
#endif