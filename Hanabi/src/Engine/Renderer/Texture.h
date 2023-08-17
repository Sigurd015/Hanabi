#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Buffer.h"
#include "Engine/Asset/Asset.h"
#include "Image.h"

#include <string>

namespace Hanabi
{
	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		TextureWrap SamplerWrap = TextureWrap::Repeat;
		TextureFilter SamplerFilter = TextureFilter::Linear;
		bool GenerateMips = true;
	};

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;
		virtual const TextureSpecification& GetSpecification() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void* GetRendererID() const = 0;
		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void SetData(Buffer data) = 0;
		virtual bool operator==(const Texture& other) const = 0;

		virtual TextureType GetType() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const TextureSpecification& specification, Buffer data = Buffer());

		virtual TextureType GetType() const override { return TextureType::TextureCube; }

		static AssetType GetStaticType() { return AssetType::Texture2D; }
		virtual AssetType GetAssetType() const { return GetStaticType(); }
	};

	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(const TextureSpecification& specification, Buffer data = Buffer());

		virtual TextureType GetType() const override { return TextureType::TextureCube; }

		static AssetType GetStaticType() { return AssetType::EnvMap; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	};
}