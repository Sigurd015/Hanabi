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
			PlayIcon = LoadTexture("Resources/Icons/PlayButton.png");
			StopIcon = LoadTexture("Resources/Icons/StopButton.png");
			PauseIcon = LoadTexture("Resources/Icons/PauseButton.png");
			StepIcon = LoadTexture("Resources/Icons/StepButton.png");

			DirectoryIcon = LoadTexture("Resources/Icons/ContentBrowser/DirectoryIcon.png");
			FileIcon = LoadTexture("Resources/Icons/ContentBrowser/FileIcon.png");
			ImportedFileIcon = LoadTexture("Resources/Icons/ContentBrowser/ImportedFileIcon.png");
		}
	private:
		static Ref<Texture2D> LoadTexture(const std::filesystem::path& relativePath)
		{
			return TextureSerializer::LoadTexture2D(relativePath);
		}
	};
}