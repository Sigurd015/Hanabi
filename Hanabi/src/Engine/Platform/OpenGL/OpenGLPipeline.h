#pragma once
#include "Engine/Renderer/Pipeline.h"
#include "Engine/Renderer/Shader.h"

namespace Hanabi
{
	class OpenGLPipeline : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		~OpenGLPipeline();
		void Bind() override;
		void SetConstantBuffer(Ref<ConstantBuffer> constantBuffer) override;
	private:
		uint32_t m_RendererID;
		uint32_t m_VertexBufferIndex = 0;
		Ref<ConstantBuffer> m_UniformBuffer;
	};
}