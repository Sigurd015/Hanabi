#pragma once
#include "Hanabi/Core/Base.h"

namespace Hanabi
{
	enum ShaderType
	{
		Unknown = 0, VertexShader, PixelShader, GeometryShader, ComputeShader
	};

	enum class RendererResourceType
	{
		None = 0,
		Sampler,
		Resource,
		UnorderedAccess,
		StructuredBuffer,
		ConstantBuffer,
		Count
	};

	struct ShaderResourceDeclaration
	{
		std::string Name;
		RendererResourceType ResourceType;
		uint32_t Slot;
		ShaderType Stage;
	};

	class Shader;
	class ShaderCompiler
	{
	public:
		static Scope<ShaderCompiler> Create();

		virtual Ref<Shader> Compile(const std::string& shaderName, bool disableOptimization = false) = 0;
		virtual bool TryReCompile(Ref<Shader> shader, const std::string& shaderName, bool disableOptimization = false) = 0;
	};
}