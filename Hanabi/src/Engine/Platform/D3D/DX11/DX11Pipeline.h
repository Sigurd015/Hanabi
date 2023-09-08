#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/Pipeline.h"
#include "Engine/Renderer/IndexBuffer.h"
#include "Engine/Platform/D3D/DXCommon.h"

#include <d3d11.h>

namespace Hanabi
{
	class DX11Pipeline : public Pipeline
	{
	public:
		DX11Pipeline(const PipelineSpecification& spec);
		~DX11Pipeline() = default;
		void Bind() override;

		PipelineSpecification& GetSpecification() override { return m_Specification; }
		const PipelineSpecification& GetSpecification() const override { return m_Specification; }
	private:
		ComPtr<ID3D11InputLayout> m_InputLayout;
		PipelineSpecification m_Specification;
	};
}
#endif