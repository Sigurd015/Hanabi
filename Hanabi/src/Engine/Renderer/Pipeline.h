#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "Framebuffer.h"

#include <memory>
#include <vector>

namespace Hanabi
{
	enum class PrimitiveTopology
	{
		None = 0,
		Points,
		Lines,
		Triangles,
	};

	enum class DepthCompareOperator
	{
		None = 0,
		Less,
		LessEqual,
	};

	struct PipelineSpecification
	{
		Ref<Shader> Shader;
		Ref<Framebuffer> TargetFramebuffer;
		VertexBufferLayout Layout;
		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
		DepthCompareOperator DepthOperator = DepthCompareOperator::Less;
		bool BackfaceCulling = true;
		bool DepthTest = true;
		float LineWidth = 1.0f;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual void Bind() = 0;

		virtual PipelineSpecification& GetSpecification() = 0;
		virtual const PipelineSpecification& GetSpecification() const = 0;

		static Ref<Pipeline> Create(const PipelineSpecification& spec);
	};
}