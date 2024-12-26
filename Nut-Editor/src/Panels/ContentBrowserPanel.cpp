#include "nutpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

#include <shellapi.h>

namespace Nut
{
	static float padding = 16.0f;															// �����С
	static float thumbnailSize = 128.0f;													// ����ͼ�ߴ�
	extern const std::filesystem::path g_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(g_AssetPath)
	{
		m_FolderIcon = Texture2D::Create("E:/VS/Nut/Nut-Editor/Resources/Icons/ContentBrowser/DirectoryIcon3.png");
		m_FileIcon   = Texture2D::Create("E:/VS/Nut/Nut-Editor/Resources/Icons/ContentBrowser/FileIcon3.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Contect Browser");
		
		ImGui::Columns(2);
		if(ImGui::Button("<"))																// ���ơ����ء���
		{
			if(m_CurrentDirectory != g_AssetPath)
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
			auto relativePath = std::filesystem::relative(path, g_AssetPath);				// ��¼Ŀ¼������� assets/ �����·��
			std::string& filenameString = relativePath.filename().string();					// ������·�����ļ���
			
			ImGui::PushID(filenameString.c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			Ref<Texture> icon = (directoryEntry.is_directory() ? m_FolderIcon : m_FileIcon);
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			
			if(ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROSWER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
				if (directoryEntry.is_regular_file())
				{
					std::filesystem::path absolutePath = "E:\\VS\\Nut\\Nut-Editor" / path;

					// ʹ�� ShellExecute �򿪼��±�
					ShellExecuteA(nullptr, "open", "F:\\Microsoft VS Code\\Code.exe", absolutePath.string().c_str(), nullptr, SW_SHOWNORMAL);
				}
			}

			ImGui::TextWrapped(filenameString.c_str());										// �����ı������Զ��۵���

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);
		
		ImGui::End();
	}

}