#pragma once
#include "Engine/Core/Base.h"

namespace Hanabi
{
	class ConstantBuffer
	{
	public:
		virtual ~ConstantBuffer() = default;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Bind() const = 0;
		static Ref<ConstantBuffer> Create(uint32_t size, uint32_t binding);
	};
}