#pragma once
#include "DX11/DX11Shader.h"
#include "Hanabi/Renderer/ShaderCompiler/ShaderCompiler.h"

namespace Hanabi
{
	class DXShaderCompiler : public ShaderCompiler
	{
	public:
		virtual Ref<Shader> Compile(const std::string& shaderName, bool disableOptimization = false) override;

		virtual bool TryReCompile(Ref<Shader> shader, const std::string& shaderName, bool disableOptimization = false) override;
	private:
		bool CompileShader(Ref<DX11Shader> shader, const std::string& shaderName, bool disableOptimization = false);
		void CreateReflectionData(Ref<DX11Shader> shader, ShaderType shaderType, const ComPtr<ID3DBlob>& shaderBlob);
	};
}