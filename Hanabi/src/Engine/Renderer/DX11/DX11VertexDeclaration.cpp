#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "Engine/Renderer/DX11/DX11VertexDeclaration.h"
#include "Engine/Renderer/DX11/DX11Shader.h"
#include "Engine/Renderer/DX11/DX11Context.h"

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

	DX11VertexDeclaration::DX11VertexDeclaration()
	{}

	DX11VertexDeclaration::~DX11VertexDeclaration()
	{}

	void DX11VertexDeclaration::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		m_IndexBuffer = indexBuffer;
	}

	void DX11VertexDeclaration::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, const Ref<Shader>& shader)
	{
		DX11Shader* vertexShader = (DX11Shader*)shader.get();
		const auto& layout = vertexBuffer->GetLayout();
		std::vector<D3D11_INPUT_ELEMENT_DESC> temp;
		for (const auto& element : layout)
		{
			temp.push_back(D3D11_INPUT_ELEMENT_DESC{
				element.Name.c_str(),0,ShaderDataTypeToDX11BaseType(element.Type),
				0,(UINT)element.Offset ,D3D11_INPUT_PER_VERTEX_DATA ,0 });
			m_VertexBufferIndex++;
		}

		HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateInputLayout(
			&temp[0], (UINT)temp.size(), vertexShader->GetVertextBufferPointer(),
			vertexShader->GetVertextBufferSize(), m_InputLayout.GetAddressOf()));
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void DX11VertexDeclaration::Bind() const
	{
		for (auto vertexBuffer : m_VertexBuffers)
		{
			vertexBuffer->Bind();
		}
		if (m_IndexBuffer != nullptr)
			m_IndexBuffer->Bind();
		DX11Context::GetDeviceContext()->IASetInputLayout(m_InputLayout.Get());
	}

	void DX11VertexDeclaration::Unbind() const
	{
		for (auto vertexBuffer : m_VertexBuffers)
		{
			vertexBuffer->Unbind();
		}
		if (m_IndexBuffer != nullptr)
			m_IndexBuffer->Unbind();
		DX11Context::GetDeviceContext()->IASetInputLayout(nullptr);
	}
}
#endif