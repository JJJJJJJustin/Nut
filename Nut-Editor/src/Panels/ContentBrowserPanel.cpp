#include "nutpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Nut
{
	static const std::filesystem::path s_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(s_AssetPath)
	{
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Contect Browser");
		
		if(ImGui::Button("<"))
		{
			if(m_CurrentDirectory != s_AssetPath)
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		ImGui::Separator();

		for(auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))	// m_CurrentDirectory 中的每一个目录项：DirectoryEntry
		{
			auto const& path = directoryEntry.path();										// 每一个目录项的路径
			auto relativePath = std::filesystem::relative(path, s_AssetPath);				// 记录目录项相对于 assets/ 的相对路径
			std::string& filenameString = relativePath.filename().string();					// 获得相对路径的文件名

			if (directoryEntry.is_directory()) 
			{
				if(ImGui::Button(filenameString.c_str()))
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			else
			{
				ImGui::Button(filenameString.c_str());
			}
		}

		ImGui::End();
	}

}