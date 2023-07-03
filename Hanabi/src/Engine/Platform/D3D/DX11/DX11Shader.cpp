#include "hnbpch.h"

#if defined(HNB_PLATFORM_WINDOWS)
#include "DX11Shader.h"
#include "DX11Context.h"
#include "Engine/Platform/D3D/DXCommon.h"

#include <fstream>
#include <d3dcompiler.h>
#pragma pack_matrix(row_major)

namespace Hanabi
{
	static ShaderType ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return PIXEL_SHADER;

		HNB_CORE_ASSERT(false, "Unknown shader type!");
		return UNKNOWN;
	}

	DX11Shader::DX11Shader(const std::string& fileName) :m_Name(fileName)
	{
		std::string source = ReadFile("resources/shaders/DX11/" + fileName + ".hlsl");
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
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
				HNB_CORE_ERROR("Could not read from file '{0}'", filepath);
			}
		}
		else
		{
			HNB_CORE_ERROR("Could not open file '{0}'", filepath);
		}
		return result;
	}

	std::unordered_map<ShaderType, std::string> DX11Shader::PreProcess(const std::string& source)
	{
		std::unordered_map<ShaderType, std::string> shaderSources;

		// Split shader source code into shader type strings
		{
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
		}

		// Handle #include directives
		{
			const char* includeToken = "#include \"";
			const char* fileType = ".hlsl\"";
			size_t includeTokenLength = strlen(includeToken);
			size_t fileTypeLength = strlen(fileType);
			for (auto& code : shaderSources)
			{
				size_t pos = code.second.find(includeToken, 0);

				while (pos != std::string::npos)
				{
					size_t eol = code.second.find_first_of("\r\n", pos);
					size_t begin = pos + includeTokenLength;
					std::string fileName = code.second.substr(begin, eol - begin - fileTypeLength);
					size_t nextLinePos = code.second.find_first_not_of("\r\n", eol);
					std::string includeSource = ReadFile("resources/shaders/DX11/include/" + fileName + ".hlsl");
					code.second.replace(pos, eol - pos, includeSource);
					pos = code.second.find(includeToken, nextLinePos);
				}
			}
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
				DX_CHECK_RESULT(D3DCompile(source.c_str(), source.length(), nullptr, nullptr, nullptr,
					"main", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, 0, m_VertexShaderBlob.GetAddressOf(), nullptr));
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateVertexShader(m_VertexShaderBlob->GetBufferPointer(),
					m_VertexShaderBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf()));
				break;
			}
			case Hanabi::PIXEL_SHADER:
			{
				Microsoft::WRL::ComPtr<ID3DBlob> blob;
				DX_CHECK_RESULT(D3DCompile(source.c_str(), source.length(), nullptr, nullptr, nullptr,
					"main", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, 0, blob.ReleaseAndGetAddressOf(), nullptr));
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreatePixelShader(blob->GetBufferPointer(),
					blob->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf()));
				break;
			}
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
		DX11Context::GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
		DX11Context::GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
	}

	DX11Shader::~DX11Shader()
	{
		m_VertexShader.Reset();
		m_PixelShader.Reset();
	}
}
#endif