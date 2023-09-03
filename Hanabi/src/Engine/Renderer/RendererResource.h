#pragma once

#include <string>

namespace Hanabi
{
	class RendererResource
	{
	public:
		virtual ~RendererResource() = default;
		virtual void Bind(uint32_t slot = 0) const = 0;
	};
}