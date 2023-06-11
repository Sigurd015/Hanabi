#pragma once
#include "VertexBuffer.h"
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
		Ref<RenderPass> RenderPass;
		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;
		virtual void Bind() = 0;
		PipelineSpecification& GetSpecification() { return m_Specification; }
		const PipelineSpecification& GetSpecification() const { return m_Specification; }
		virtual void SetConstantBuffer(Ref<ConstantBuffer> constantBuffer) = 0;

		static Ref<Pipeline> Create(const PipelineSpecification& spec);
	protected:
		PipelineSpecification m_Specification;
	};
}