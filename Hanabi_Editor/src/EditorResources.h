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
			PlayIcon = TextureSerializer::LoadTexture2D("Resources/Icons/PlayButton.png");
			StopIcon = TextureSerializer::LoadTexture2D("Resources/Icons/StopButton.png");
			PauseIcon = TextureSerializer::LoadTexture2D("Resources/Icons/PauseButton.png");
			StepIcon = TextureSerializer::LoadTexture2D("Resources/Icons/StepButton.png");

			DirectoryIcon = TextureSerializer::LoadTexture2D("Resources/Icons/ContentBrowser/DirectoryIcon.png");
			FileIcon = TextureSerializer::LoadTexture2D("Resources/Icons/ContentBrowser/FileIcon.png");
			ImportedFileIcon = TextureSerializer::LoadTexture2D("Resources/Icons/ContentBrowser/ImportedFileIcon.png");
		}
	};
}