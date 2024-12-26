#include "nutpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

#include <shellapi.h>

namespace Nut
{
	static float padding = 16.0f;															// 间隔大小
	static float thumbnailSize = 128.0f;													// 缩略图尺寸
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
		if(ImGui::Button("<"))																// 绘制“返回”键
		{
			if(m_CurrentDirectory != g_AssetPath)
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		ImGui::NextColumn();
		ImGui::SliderFloat("ThumbnailSize", &thumbnailSize, 10, 512);						// 绘制滑块
		ImGui::SliderFloat("Padding", &padding, 0, 32);
		ImGui::Columns(1);

		ImGui::Separator();

		float cellSize = padding + thumbnailSize;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);												// 设置合适的列数

		for(auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))	// m_CurrentDirectory 中的每一个目录项：DirectoryEntry
		{
			auto const& path = directoryEntry.path();										// 每一个目录项的路径
			auto relativePath = std::filesystem::relative(path, g_AssetPath);				// 记录目录项相对于 assets/ 的相对路径
			std::string& filenameString = relativePath.filename().string();					// 获得相对路径的文件名
			
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

					// 使用 ShellExecute 打开记事本
					ShellExecuteA(nullptr, "open", "F:\\Microsoft VS Code\\Code.exe", absolutePath.string().c_str(), nullptr, SW_SHOWNORMAL);
				}
			}

			ImGui::TextWrapped(filenameString.c_str());										// 附加文本（可自动折叠）

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);
		
		ImGui::End();
	}

}