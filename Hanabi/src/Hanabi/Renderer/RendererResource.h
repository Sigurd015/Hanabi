#pragma once
#include "ShaderCompiler/ShaderCompiler.h"

#include <string>

namespace Hanabi
{
	class RendererResource
	{
	public:	
		virtual ~RendererResource() = default;
		virtual void Bind(const ShaderResourceDeclaration& declaration) const = 0;
	};
}