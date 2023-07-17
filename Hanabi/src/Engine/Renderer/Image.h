#pragma once

namespace Hanabi
{
	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F,

		// Color
		RGBA8F,

		// Depth/stencil
		DEPTH24STENCIL8,
		DEPTH32F,

		// TODO: Make a better way to do mouse picking
		RED8UI,
		MousePick = RED8UI,

		// Defaults
		Depth = DEPTH24STENCIL8,
		ShadowMap = DEPTH32F
	};
}
