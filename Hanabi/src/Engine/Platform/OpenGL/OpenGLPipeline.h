#pragma once
#include "Engine/Renderer/Pipeline.h"
#include "Engine/Renderer/Shader.h"

namespace Hanabi
{
	//Do the VertexArray work
	class OpenGLPipeline : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		~OpenGLPipeline();
		void Bind() override;
		void SetConstantBuffer(const Ref<ConstantBuffer>& constantBuffer) override;

		PipelineSpecification& GetSpecification() override { return m_Specification; }
		const PipelineSpecification& GetSpecification() const override { return m_Specification; }
	private:
		uint32_t m_RendererID = 0;

		Ref<ConstantBuffer> m_UniformBuffer;
		PipelineSpecification m_Specification;
	};
}