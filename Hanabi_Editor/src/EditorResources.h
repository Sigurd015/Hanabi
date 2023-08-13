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
			PlayIcon = LoadTexture("resources/icons/PlayButton.png");
			StopIcon = LoadTexture("resources/icons/StopButton.png");
			PauseIcon = LoadTexture("resources/icons/PauseButton.png");
			StepIcon = LoadTexture("resources/icons/StepButton.png");

			DirectoryIcon = LoadTexture("resources/icons/ContentBrowser/DirectoryIcon.png");
			FileIcon = LoadTexture("resources/icons/ContentBrowser/FileIcon.png");
			ImportedFileIcon = LoadTexture("resources/icons/ContentBrowser/ImportedFileIcon.png");
		}

		static void Shutdown()
		{}

	private:
		static Ref<Texture2D> LoadTexture(const std::filesystem::path& relativePath)
		{
			return TextureSerializer::LoadTexture2D(relativePath);
		}
	};

}
