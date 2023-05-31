#pragma once
#include "Hanabi.h"

#include <filesystem>

namespace Hanabi
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		void SetPath(std::filesystem::path projPath);
		void OnImGuiRender();
	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};
}