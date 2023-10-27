#pragma once
#include "RendererResource.h"

#include <glm/gtc/integer.hpp>

namespace Hanabi
{
	enum class ImageFormat
	{
		// Unsigned normalized
		None = 0,
		R8,
		RGB8,
		RGBA8,

		RED8UI,

		// Color - signed float
		RGBA8F,
		RGBA16F,
		RGBA32F,

		// Depth/stencil
		DEPTH24STENCIL8,
		DEPTH32F,

		// Defaults
		Depth = DEPTH24STENCIL8,
		ShadowMap = DEPTH32F
	};

	enum class ImageUsage
	{
		None = 0,
		Texture2D,
		Attachment,
	};

	struct ImageSpecification
	{
		ImageFormat Format = ImageFormat::RGBA8;
		ImageUsage Usage = ImageUsage::Texture2D;
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Mips = 1;
	};

	class Image : public RendererResource
	{
	public:
		virtual ~Image() = default;

		virtual void Resize(const glm::uvec2& size) = 0;
		virtual void Resize(const uint32_t width, const uint32_t height) = 0;
		virtual void Invalidate() = 0;
		virtual void Release() = 0;
		virtual void Bind(uint32_t slot) const = 0;
		virtual void* GetRendererID() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual glm::uvec2 GetSize() const = 0;

		virtual float GetAspectRatio() const = 0;

		virtual ImageSpecification& GetSpecification() = 0;
		virtual const ImageSpecification& GetSpecification() const = 0;

		virtual Buffer GetBuffer() const = 0;
		virtual Buffer& GetBuffer() = 0;
	};

	class Image2D : public Image
	{
	public:
		static Ref<Image2D> Create(const ImageSpecification& specification, Buffer buffer = Buffer());
	};

	namespace Utils
	{
		inline uint32_t CalculateMipCount(uint32_t width, uint32_t height)
		{
			return (uint32_t)glm::floor(glm::log2(glm::min(width, height))) + 1;
		}

		inline bool IsDepthFormat(ImageFormat format)
		{
			if (format == ImageFormat::DEPTH24STENCIL8 || format == ImageFormat::DEPTH32F)
				return true;

			return false;
		}
	}
}
