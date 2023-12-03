#pragma once
#include "Engine/Renderer/Shader.h"

#include <string>

namespace Hanabi
{
	class RendererResource
	{
	public:
		virtual ~RendererResource() = default;
		virtual void Bind(uint32_t slot = 0) const = 0;
	};

	namespace Utils
	{
		template<typename BindFunction>
		inline void BindResource(const ShaderReflectionData& reflectionData, const std::string& resourceName, BindFunction function)
		{
			auto it = reflectionData.find(resourceName);
			if (it != reflectionData.end())
			{
				function(it->second);
			}
			else
			{
				HNB_CORE_WARN("RendererResource {} not found in shader!", resourceName);
			}
		}
	}
}