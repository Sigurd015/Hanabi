#pragma once
#include "Framebuffer.h"
#include "Pipeline.h"
#include "Texture.h"
#include "ConstantBuffer.h"

namespace Hanabi
{
	struct RenderPassSpecification
	{
		Ref<Pipeline> Pipeline;
	};

	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		virtual void SetInput(std::string_view name, Ref<ConstantBuffer> constantBuffer) = 0;
		virtual void SetInput(std::string_view name, Ref<TextureCube> textureCube) = 0;
		virtual void SetInput(std::string_view name, Ref<Texture2D> texture) = 0;

		virtual void BindInputs() = 0;

		virtual Ref<Texture2D> GetOutput(uint32_t index) = 0;
		virtual Ref<Texture2D> GetDepthOutput() = 0;

		virtual RenderPassSpecification& GetSpecification() = 0;
		virtual const RenderPassSpecification& GetSpecification() const = 0;

		virtual Ref<Pipeline> GetPipeline() const = 0;
		virtual Ref<Framebuffer> GetTargetFramebuffer() const = 0;

		static Ref<RenderPass> Create(const RenderPassSpecification& spec);
	};
}