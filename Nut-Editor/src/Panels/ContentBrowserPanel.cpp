#include "nutpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Nut
{
	static float padding = 16.0f;															// �����С
	static float thumbnailSize = 128.0f;													// ����ͼ�ߴ�
	static const std::filesystem::path s_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(s_AssetPath)
	{
		m_FolderIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon3.png");
		m_FileIcon   = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon3.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Contect Browser");
		
		ImGui::Columns(2);
		if(ImGui::Button("<"))																// ���ơ����ء���
		{
			if(m_CurrentDirectory != s_AssetPath)
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		ImGui::NextColumn();
		ImGui::SliderFloat("ThumbnailSize", &thumbnailSize, 10, 512);						// ���ƻ���
		ImGui::SliderFloat("Padding", &padding, 0, 32);
		ImGui::Columns(1);

		ImGui::Separator();

		float cellSize = padding + thumbnailSize;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);												// ���ú��ʵ�����

		for(auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))	// m_CurrentDirectory �е�ÿһ��Ŀ¼�DirectoryEntry
		{
			auto const& path = directoryEntry.path();										// ÿһ��Ŀ¼���·��
			auto relativePath = std::filesystem::relative(path, s_AssetPath);				// ��¼Ŀ¼������� assets/ �����·��
			std::string& filenameString = relativePath.filename().string();					// ������·�����ļ���
			
			Ref<Texture> icon = (directoryEntry.is_directory() ? m_FolderIcon : m_FileIcon);
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			ImGui::TextWrapped(filenameString.c_str());										// �����ı������Զ��۵���

			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		
		ImGui::End();
	}

}