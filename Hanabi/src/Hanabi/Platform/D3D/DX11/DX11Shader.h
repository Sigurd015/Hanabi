#ifdef HNB_ENABLE_D3D
#pragma once
#include "Hanabi/Renderer/Shader.h"
#include "Hanabi/Platform/D3D/DXCommon.h"

#include <string>
#include <d3d11.h>

namespace Hanabi
{
	class DX11Shader :public Shader
	{
	public:
		DX11Shader() = default;
		~DX11Shader();
		void Bind() const override;
		virtual const std::vector<ShaderResourceDeclaration>& GetReflectionData() const override { return m_ReflectionData; }
		LPVOID GetVertextBufferPointer() { return m_VertexShaderBlob->GetBufferPointer(); }
		SIZE_T GetVertextBufferSize() { return m_VertexShaderBlob->GetBufferSize(); }
	private:
		ComPtr<ID3D11VertexShader> m_VertexShader;
		ComPtr<ID3D11GeometryShader> m_GeometryShader;
		ComPtr<ID3D11PixelShader> m_PixelShader;
		ComPtr<ID3D11ComputeShader> m_ComputeShader;
		ComPtr<ID3DBlob> m_VertexShaderBlob;
		std::vector<ShaderResourceDeclaration> m_ReflectionData;

		friend class DXShaderCompiler;
	};
}
#endif