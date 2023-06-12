#include "hnbpch.h"
#include "DX11Pipeline.h"
#include "DX11Shader.h"
#include "DX11Context.h"
#include "Engine/Platform/D3D/DXCommon.h"

#include <vector>

namespace Hanabi
{
	static DXGI_FORMAT ShaderDataTypeToDX11BaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2:   return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3:   return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4:   return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Mat3:     return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Mat4:     return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Int:      return DXGI_FORMAT_R32_SINT;
		case ShaderDataType::Int2:     return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3:     return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4:     return DXGI_FORMAT_R32G32B32A32_SINT;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	DX11Pipeline::DX11Pipeline(const PipelineSpecification& spec)
	{
		m_Specification = spec;
		DX11Shader* vertexShader = (DX11Shader*)m_Specification.Shader.get();
		const auto& layout = m_Specification.Layout;
		std::vector<D3D11_INPUT_ELEMENT_DESC> temp;
		for (const auto& element : layout)
		{
			temp.push_back(D3D11_INPUT_ELEMENT_DESC{
				element.Name.c_str(),0,ShaderDataTypeToDX11BaseType(element.Type),
				0,(UINT)element.Offset ,D3D11_INPUT_PER_VERTEX_DATA ,0 });
		}

		DX_CHECK_RESULT(DX11Context::GetDevice()->CreateInputLayout(
			&temp[0], (UINT)temp.size(), vertexShader->GetVertextBufferPointer(),
			vertexShader->GetVertextBufferSize(), m_InputLayout.GetAddressOf()));
	}

	void DX11Pipeline::Bind()
	{
		DX11Context::GetDeviceContext()->IASetInputLayout(m_InputLayout.Get());
		if (m_ConstantBuffer != nullptr)
			m_ConstantBuffer->Bind();
	}

	void DX11Pipeline::SetConstantBuffer(Ref<ConstantBuffer> constantBuffer)
	{
		m_ConstantBuffer = constantBuffer;
	}
}