#pragma once

#include <string>

namespace Hanabi
{
	enum class RendererResourceType
	{
		Texture2D,
		TextureCube,
		ConstantBuffer,
	};

	class RendererResource : public std::enable_shared_from_this<RendererResource>
	{
	public:
		virtual ~RendererResource() = default;

		template <typename T>
		Ref<T> GetAs()
		{
			static_assert(std::is_base_of<RendererResource, T>::value, "RendererResource::GetAs only works for types derived from RendererResource");
			return std::dynamic_pointer_cast<T>(shared_from_this());
		}

		virtual RendererResourceType GetRendererResourceType() const = 0;
	};
}