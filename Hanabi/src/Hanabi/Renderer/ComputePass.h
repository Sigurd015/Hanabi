#pragma once
#include "Hanabi/Core/Base.h"
#include "Shader.h"
#include "ComputePipeline.h"
#include "RendererResource.h"

namespace Hanabi
{
	struct ComputePassSpecification
	{
		Ref<ComputePipeline> Pipeline;
	};

	class ComputePass
	{
	public:
		virtual ~ComputePass() = default;

		virtual void SetInput(std::string_view name, Ref<RendererResource> resource) = 0;
		virtual void BindInputs() = 0;
		virtual void UnbindInputs() = 0;
		virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z) = 0;

		virtual Ref<ComputePipeline> GetPipeline() const = 0;
		virtual ComputePassSpecification& GetSpecification() = 0;
		virtual const ComputePassSpecification& GetSpecification() const = 0;

		static Ref<ComputePass> Create(const ComputePassSpecification& spec);
	};
}