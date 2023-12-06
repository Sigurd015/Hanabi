#pragma once
#include <vector>
#include <msdf-atlas-gen.h>

namespace Hanabi
{
	struct MSDFData
	{
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
		msdf_atlas::FontGeometry FontGeometry;
	};
}