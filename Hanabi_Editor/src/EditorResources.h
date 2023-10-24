#pragma once
#include "Hanabi.h"

namespace Hanabi
{
	class EditorResources
	{
	public:
		inline static Ref<Texture2D> PlayIcon = nullptr;
		inline static Ref<Texture2D> StopIcon = nullptr;
		inline static Ref<Texture2D> PauseIcon = nullptr;
		inline static Ref<Texture2D> StepIcon = nullptr;

		inline static Ref<Texture2D> DirectoryIcon = nullptr;
		inline static Ref<Texture2D> FileIcon = nullptr;
		inline static Ref<Texture2D> ImportedFileIcon = nullptr;

		static void Init()
		{
			TextureSpecification spec = {};
			spec.GenerateMips = false;

			PlayIcon = TextureSerializer::LoadTexture2D("Resources/Icons/PlayButton.png", spec);
			StopIcon = TextureSerializer::LoadTexture2D("Resources/Icons/StopButton.png", spec);
			PauseIcon = TextureSerializer::LoadTexture2D("Resources/Icons/PauseButton.png", spec);
			StepIcon = TextureSerializer::LoadTexture2D("Resources/Icons/StepButton.png", spec);

			DirectoryIcon = TextureSerializer::LoadTexture2D("Resources/Icons/ContentBrowser/DirectoryIcon.png", spec);
			FileIcon = TextureSerializer::LoadTexture2D("Resources/Icons/ContentBrowser/FileIcon.png", spec);
			ImportedFileIcon = TextureSerializer::LoadTexture2D("Resources/Icons/ContentBrowser/ImportedFileIcon.png", spec);
		}
	};
}