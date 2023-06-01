#include "hnbpch.h"
#include "Engine/Renderer/Font.h"

#define MSDF_ATLAS_NO_ARTERY_FONT
#include <msdf-atlas-gen.h>

namespace Hanabi
{
	Font::Font(const std::filesystem::path& path)
	{
		msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
		if (ft)
		{
			std::string fileString = path.string();
			msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
			if (font)
			{
				msdfgen::Shape shape;
				if (msdfgen::loadGlyph(shape, font, 'C'))
				{
					shape.normalize();				
					msdfgen::edgeColoringSimple(shape, 3.0); //max. angle			
					msdfgen::Bitmap<float, 3> msdf(32, 32);	//image width, height			
					msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0)); //range, scale, translation
					msdfgen::savePng(msdf, "output.png");
				}
				msdfgen::destroyFont(font);
			}
			msdfgen::deinitializeFreetype(ft);
		}
	}
}