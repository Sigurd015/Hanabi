#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "Engine/Renderer/DX11/DX11Shader.h"
#include "Engine/Renderer/DX11/DX11Context.h"

#include <fstream>
#include <d3dcompiler.h>

namespace Hanabi
{
	static ShaderType ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return PIXEL_SHADER;

		return UNKNOWN;
	}

	DX11Shader::DX11Shader(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	std::string DX11Shader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
				in.close();
			}
			else
			{
				HNB_CORE_ASSERT("[Shader]Could not read from file", filepath);
			}
		}
		else
		{
			HNB_CORE_ASSERT("[Shader]Could not open file", filepath);
		}
		return result;
	}

	std::unordered_map<ShaderType, std::string> DX11Shader::PreProcess(const std::string& source)
	{

		std::unordered_map<ShaderType, std::string> shaderSources;
		const char* typeToken = "#type:";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			size_t begin = pos + typeTokenLength; //Start of shader type name (after "#type:" keyword)
			std::string type = source.substr(begin, eol - begin);
			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line
			shaderSources[ShaderTypeFromString(type)] =
				(pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}
		return shaderSources;
	}

	void DX11Shader::Compile(const std::unordered_map<ShaderType, std::string>& shaderSources)
	{

		for (auto& kv : shaderSources)
		{
			ShaderType type = kv.first;
			const std::string& source = kv.second;
			switch (type)
			{
			case Hanabi::VERTEX_SHADER:
			{
				HNB_CORE_DX_ASSERT(D3DCompile(source.c_str(), source.length(), nullptr, nullptr, nullptr,
					"main", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, m_VertexShaderBlob.GetAddressOf(), nullptr));
				HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreateVertexShader(m_VertexShaderBlob->GetBufferPointer(),
					m_VertexShaderBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf()));
				break;
			}
			case Hanabi::PIXEL_SHADER:
			{
				Microsoft::WRL::ComPtr<ID3DBlob> blob;
				HNB_CORE_DX_ASSERT(D3DCompile(source.c_str(), source.length(), nullptr, nullptr, nullptr,
					"main", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, blob.ReleaseAndGetAddressOf(), nullptr));
				HNB_CORE_DX_ASSERT(DX11Context::GetDevice()->CreatePixelShader(blob->GetBufferPointer(),
					blob->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf()));
				break;
			}
			case Hanabi::UNKNOWN:
			default:
				HNB_CORE_ASSERT("[Shader]Unknown Shader Type!");
				break;
			}
		}
	}

	void DX11Shader::Bind() const
	{
		DX11Context::GetDeviceContext()->VSSetShader(m_VertexShader.Get(), nullptr, 0);
		DX11Context::GetDeviceContext()->PSSetShader(m_PixelShader.Get(), nullptr, 0);
	}

	void DX11Shader::Unbind() const
	{

	}

	void DX11Shader::SetUniform(const std::string& name, int value)
	{}

	void DX11Shader::SetUniform(const std::string& name, int* values, uint32_t count)
	{}

	void DX11Shader::SetUniform(const std::string& name, float value)
	{}

	void DX11Shader::SetUniform(const std::string& name, const glm::vec2& value)
	{}

	void DX11Shader::SetUniform(const std::string& name, const glm::vec3& value)
	{}

	void DX11Shader::SetUniform(const std::string& name, const glm::vec4& value)
	{}

	void DX11Shader::SetUniform(const std::string& name, const glm::mat4& value)
	{}

	DX11Shader::~DX11Shader()
	{}
}
#endif