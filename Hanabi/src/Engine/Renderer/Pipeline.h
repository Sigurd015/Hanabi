#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "RenderPass.h"

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

	struct PipelineSpecification
	{
		Ref<Shader> Shader;
		VertexBufferLayout Layout;
		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
		float LineWidth = 1.0f;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual void Bind() = 0;

		virtual PipelineSpecification& GetSpecification() = 0;
		virtual const PipelineSpecification& GetSpecification() const = 0;

		virtual void SetConstantBuffer(const Ref<ConstantBuffer>& constantBuffer) = 0;
		virtual Ref<ConstantBuffer> GetConstantBuffer(uint32_t bindingID = 0) = 0;

		static Ref<Pipeline> Create(const PipelineSpecification& spec);
	};
}