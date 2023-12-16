#pragma once
#include "Hanabi/Core/Base.h"
#include "Shader.h"

namespace Hanabi
{
	struct ComputePipelineSpecification
	{
		Ref<Shader> Shader;
	};

	class ComputePipeline
	{
	public:
		virtual ~ComputePipeline() = default;

		virtual void Bind() = 0;

		virtual ComputePipelineSpecification& GetSpecification() = 0;
		virtual const ComputePipelineSpecification& GetSpecification() const = 0;

		static Ref<ComputePipeline> Create(const ComputePipelineSpecification& spec);
	};
}