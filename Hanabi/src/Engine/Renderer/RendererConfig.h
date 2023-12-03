#pragma once
#include "RendererAPI.h"

#include <string>

namespace Hanabi
{
	struct RendererConfig
	{
		RendererAPIType APIType;

		bool ComputeEnvironmentMaps = true;

		// Tiering settings
		uint32_t EnvironmentMapResolution = 1024;
		uint32_t IrradianceMapComputeSamples = 512;

		std::string ShaderPackPath;
	};

}