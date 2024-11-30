#pragma once

#include <filesystem>

#include "Nut/Renderer/Texture.h"

namespace Nut
{

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture> m_FolderIcon;
		Ref<Texture> m_FileIcon;
	};

}