#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/Pipeline.h"
#include "Engine/Renderer/IndexBuffer.h"

#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>

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
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
		PipelineSpecification m_Specification;
	};
}
#endif