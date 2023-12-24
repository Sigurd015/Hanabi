#include "hnbpch.h"
#include "DXShaderCompiler.h"
#include "Hanabi/Renderer/ShaderCompiler/ShaderPreprocessing/ShaderPreprocessor.h"
#include "Hanabi/Core/FileSystem.h"
#include "DX11/DX11Context.h"

#include <d3dcompiler.h>
#include <d3d11Shader.h>

namespace Hanabi
{
	Ref<Shader> DXShaderCompiler::Compile(const std::string& shaderName, bool disableOptimization)
	{
		std::vector<ShaderSource> shaderSource = ShaderPreprocessor::Preprocess(shaderName);

		Ref<DX11Shader> shader = CreateRef<DX11Shader>();

		bool success = CompileShader(shader, shaderName, disableOptimization);

		if (success)
			return shader;
		else
			HNB_CORE_ASSERT(false, "Failed to compile shader!");
	}

	bool DXShaderCompiler::TryReCompile(Ref<Shader> shader, const std::string& shaderName, bool disableOptimization)
	{
		std::vector<ShaderSource> shaderSource = ShaderPreprocessor::Preprocess(shaderName);

		Ref<DX11Shader> dxShader = std::static_pointer_cast<DX11Shader>(shader);

		return CompileShader(dxShader, shaderName, disableOptimization);
	}

	bool DXShaderCompiler::CompileShader(Ref<DX11Shader> shader, const std::string& shaderName, bool disableOptimization)
	{
		std::vector<ShaderSource> shaderSource = ShaderPreprocessor::Preprocess(shaderName);

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
		if (disableOptimization)
			flags |= D3DCOMPILE_SKIP_OPTIMIZATION;

		shader->m_ReflectionData.clear();
		for (auto& data : shaderSource)
		{
			switch (data.Type)
			{
			case VertexShader:
			{
				if (!CheckD3DError(D3DCompile(data.Source.c_str(), data.Source.length(), nullptr, nullptr, nullptr,
					"main", "vs_5_0", flags, 0, shader->m_VertexShaderBlob.GetAddressOf(), nullptr)))
				{
					return false;
				}
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateVertexShader(shader->m_VertexShaderBlob->GetBufferPointer(),
					shader->m_VertexShaderBlob->GetBufferSize(), nullptr, shader->m_VertexShader.GetAddressOf()));

				CreateReflectionData(shader, VertexShader, shader->m_VertexShaderBlob);
				break;
			}
			case GeometryShader:
			{
				ComPtr<ID3DBlob> blob;
				if (!CheckD3DError(D3DCompile(data.Source.c_str(), data.Source.length(), nullptr, nullptr, nullptr,
					"main", "gs_5_0", flags, 0, blob.ReleaseAndGetAddressOf(), nullptr)))
				{
					return false;
				}
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateGeometryShader(blob->GetBufferPointer(),
					blob->GetBufferSize(), nullptr, shader->m_GeometryShader.GetAddressOf()));

				CreateReflectionData(shader, GeometryShader, blob);
				break;
			}
			case PixelShader:
			{
				ComPtr<ID3DBlob> blob;
				if (!CheckD3DError(D3DCompile(data.Source.c_str(), data.Source.length(), nullptr, nullptr, nullptr,
					"main", "ps_5_0", flags, 0, blob.ReleaseAndGetAddressOf(), nullptr)))
				{
					return false;
				}
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreatePixelShader(blob->GetBufferPointer(),
					blob->GetBufferSize(), nullptr, shader->m_PixelShader.GetAddressOf()));

				CreateReflectionData(shader, PixelShader, blob);
				break;
			}
			case ComputeShader:
			{
				ComPtr<ID3DBlob> blob;
				if (!CheckD3DError(D3DCompile(data.Source.c_str(), data.Source.length(), nullptr, nullptr, nullptr,
					"main", "cs_5_0", flags, 0, blob.ReleaseAndGetAddressOf(), nullptr)))
				{
					return false;
				}
				DX_CHECK_RESULT(DX11Context::GetDevice()->CreateComputeShader(blob->GetBufferPointer(),
					blob->GetBufferSize(), nullptr, shader->m_ComputeShader.GetAddressOf()));

				CreateReflectionData(shader, ComputeShader, blob);
				break;
			}
			}
		}

		return true;
	}

	void DXShaderCompiler::CreateReflectionData(Ref<DX11Shader> shader, ShaderType shaderType, const ComPtr<ID3DBlob>& shaderBlob)
	{
		ComPtr<ID3D11ShaderReflection> shaderReflection;
		DX_CHECK_RESULT(D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&shaderReflection));

		D3D11_SHADER_DESC shaderDesc;
		shaderReflection->GetDesc(&shaderDesc);

		for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc;
			shaderReflection->GetResourceBindingDesc(i, &bindDesc);

			// D3D_SIT_CBUFFER --> Constant Buffer
			// D3D_SIT_TEXTURE --> Texture
			// D3D_SIT_SAMPLER --> Sampler
			// D3D_SIT_STRUCTURED --> Structured Buffer
			// D3D_SIT_UAV_RWTYPED --> RWTexture2D/Texture2DArray
			if (bindDesc.Type == D3D_SIT_CBUFFER)
			{
				shader->m_ReflectionData.push_back(
					{ bindDesc.Name,RendererResourceType::ConstantBuffer,bindDesc.BindPoint,shaderType });
			}
			else if (bindDesc.Type == D3D_SIT_TEXTURE)
			{
				shader->m_ReflectionData.push_back(
					{ bindDesc.Name,RendererResourceType::Resource,bindDesc.BindPoint,shaderType });
			}
			else if (bindDesc.Type == D3D_SIT_SAMPLER)
			{
				shader->m_ReflectionData.push_back(
					{ bindDesc.Name,RendererResourceType::Sampler,bindDesc.BindPoint,shaderType });
			}
			else if (bindDesc.Type == D3D_SIT_STRUCTURED)
			{
				shader->m_ReflectionData.push_back(
					{ bindDesc.Name,RendererResourceType::StructuredBuffer,bindDesc.BindPoint,shaderType });
			}
			else if (bindDesc.Type == D3D_SIT_UAV_RWTYPED)
			{
				shader->m_ReflectionData.push_back(
					{ bindDesc.Name,RendererResourceType::UnorderedAccess,bindDesc.BindPoint,shaderType });
			}
		}
	}
}