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

		for(auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))	// m_CurrentDirectory �е�ÿһ��Ŀ¼�DirectoryEntry
		{
			auto const& path = directoryEntry.path();										// ÿһ��Ŀ¼���·��
			auto relativePath = std::filesystem::relative(path, s_AssetPath);				// ��¼Ŀ¼������� assets/ �����·��
			std::string& filenameString = relativePath.filename().string();					// ������·�����ļ���

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