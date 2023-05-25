#if defined(HNB_PLATFORM_WINDOWS)
#pragma once
#include "Engine/Renderer/Shader.h"

#include <string>
#include <d3d11.h>
#include <wrl.h>

namespace Hanabi
{
	enum ShaderType
	{
		UNKNOWN = 0, VERTEX_SHADER, PIXEL_SHADER
	};

	class DX11Shader :public Shader
	{
	public:
		DX11Shader(const std::string& filepath);
		~DX11Shader();
		void Bind() const override;
		void Unbind() const override;
		const std::string& GetName() const override { return m_Name; }
		void SetUniform(const std::string& name, int value) override;
		void SetUniform(const std::string& name, int* values, uint32_t count) override;
		void SetUniform(const std::string& name, float value)override;
		void SetUniform(const std::string& name, const glm::vec2& value) override;
		void SetUniform(const std::string& name, const glm::vec3& value) override;
		void SetUniform(const std::string& name, const glm::vec4& value) override;
		void SetUniform(const std::string& name, const glm::mat4& value) override;
		LPVOID GetVertextBufferPointer() { return m_VertexShaderBlob->GetBufferPointer(); }
		SIZE_T GetVertextBufferSize() { return m_VertexShaderBlob->GetBufferSize(); }
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<ShaderType, std::string>& shaderSources);
		std::string m_Name;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
		Microsoft::WRL::ComPtr<ID3DBlob> m_VertexShaderBlob;
	};
}
#endif