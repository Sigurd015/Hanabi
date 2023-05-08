#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "Engine/Renderer/DX11/DX11VertexArray.h"
#include "Engine/Renderer/DX11/DX11Shader.h"
#include "Engine/Renderer/DX11/DX11Context.h"

#include <vector>
#include <d3d11.h>

namespace Hanabi
{
	static DXGI_FORMAT ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2:   return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3:   return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4:   return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Mat3:     return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Mat4:     return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Int:      return DXGI_FORMAT_R32_UINT;
		case ShaderDataType::Int2:     return DXGI_FORMAT_R32G32_UINT;
		case ShaderDataType::Int3:     return DXGI_FORMAT_R32G32B32_UINT;
		case ShaderDataType::Int4:     return DXGI_FORMAT_R32G32B32A32_UINT;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	DX11VertexArray::DX11VertexArray()
	{}

	DX11VertexArray::~DX11VertexArray()
	{}

	void DX11VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}

	void DX11VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, const Ref<Shader>& shader)
	{
		vertexBuffer->Bind();
		DX11Shader* vertexShader = (DX11Shader*)shader.get();
		const auto& layout = vertexBuffer->GetLayout();
		std::vector<D3D11_INPUT_ELEMENT_DESC> temp;
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				temp.push_back(D3D11_INPUT_ELEMENT_DESC{
						element.Name.c_str(),0,ShaderDataTypeToOpenGLBaseType(element.Type),
						0,(UINT)element.Offset ,D3D11_INPUT_PER_VERTEX_DATA ,0 });
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				break;
			}
			default:
				break;
			}
		}

		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
		DX11Context::GetDevice()->CreateInputLayout(
			&temp[0], (UINT)temp.size(), vertexShader->GetVertextBufferPointer(),
			vertexShader->GetVertextBufferSize(), inputLayout.GetAddressOf());
		DX11Context::GetDeviceContext()->IASetInputLayout(inputLayout.Get());
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void DX11VertexArray::Bind() const
	{

	}

	void DX11VertexArray::Unbind() const
	{

	}
}
#endif