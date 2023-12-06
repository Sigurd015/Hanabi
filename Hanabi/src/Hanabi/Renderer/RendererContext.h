#pragma once
#include "Hanabi/Core/Base.h"

namespace Hanabi
{
	class RendererContext
	{
	public:
		virtual void SwapBuffer(bool VSync) = 0;
		static Scope<RendererContext> Create(void* window);
	};
}