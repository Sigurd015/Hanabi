#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/Shader.h"
#include "Engine/Platform/D3D/DXCommon.h"

#include <string>
#include <d3d11.h>

namespace Hanabi
{
	enum ShaderType
	{
		UNKNOWN = 0, VERTEX_SHADER, PIXEL_SHADER
	};

	class DX11Shader :public Shader
	{
	public:
		DX11Shader(const std::string& fileName);
		~DX11Shader();
		void Bind() const override;
		void Unbind() const override;
		const std::string& GetName() const override { return m_Name; }
		virtual const ShaderReflectionData& GetReflectionData() const override { return m_ReflectionData; }
		LPVOID GetVertextBufferPointer() { return m_VertexShaderBlob->GetBufferPointer(); }
		SIZE_T GetVertextBufferSize() { return m_VertexShaderBlob->GetBufferSize(); }
	private:
		void CreateReflectionData(const ComPtr<ID3DBlob>& shaderBlob);
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<ShaderType, std::string>& shaderSources);
		std::string m_Name;
		ComPtr<ID3D11VertexShader> m_VertexShader;
		ComPtr<ID3D11PixelShader> m_PixelShader;
		ComPtr<ID3DBlob> m_VertexShaderBlob;
		ShaderReflectionData m_ReflectionData;
	};
}
#endif