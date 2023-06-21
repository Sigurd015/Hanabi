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
		void SetConstantBuffer(const Ref<ConstantBuffer>& constantBuffer) override;
		Ref<ConstantBuffer> GetConstantBuffer(uint32_t bindingID) override;

		PipelineSpecification& GetSpecification() override { return m_Specification; }
		const PipelineSpecification& GetSpecification() const override { return m_Specification; }
	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
		PipelineSpecification m_Specification;
		std::vector<Ref<ConstantBuffer>> m_ConstantBuffers;
	};
}