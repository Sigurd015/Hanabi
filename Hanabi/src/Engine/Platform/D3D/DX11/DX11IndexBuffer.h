#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/IndexBuffer.h"
#include "Engine/Platform/D3D/DXCommon.h"

#include <cstdint>
#include <d3d11.h>

namespace Hanabi
{
	class DX11IndexBuffer : public IndexBuffer
	{
	public:
		DX11IndexBuffer(void* indices, uint32_t count);
		~DX11IndexBuffer();
		void Bind() const override;
		uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_Count;
		ComPtr<ID3D11Buffer> m_IndexBuffer;
	};
}
#endif