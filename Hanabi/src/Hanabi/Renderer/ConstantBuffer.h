#pragma once
#include "Hanabi/Core/Base.h"
#include "RendererResource.h"

namespace Hanabi
{
	class ConstantBuffer : public RendererResource
	{
	public:
		virtual ~ConstantBuffer() = default;
		virtual void SetData(const void* data, uint32_t offset = 0) = 0;
		virtual void Bind(const ShaderResourceDeclaration& declaration) const = 0;

		static Ref<ConstantBuffer> Create(uint32_t size);
	};
}