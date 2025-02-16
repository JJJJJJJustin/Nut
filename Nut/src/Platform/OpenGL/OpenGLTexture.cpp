#include "nutpch.h"
#include "Platform/OpenGL/OpenGLTexture.h"


#include <stb_image.h>

namespace Nut
{
	// !!! This Function needs to be used with "SetData" !!!
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		:m_Width(width),m_Height(height)
	{
		NUT_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);			// 储存数据(width 和 height 表示像素点个数形成的宽与高）

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);				//纹理过滤
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);					//纹理环绕模式
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_Path(path)
	{
		NUT_PROFILE_FUNCTION();

		stbi_set_flip_vertically_on_load(1);

		int width, height, channels;
		stbi_uc* data = nullptr;
		{
			NUT_PROFILE_SCOPE("stbi_load -> OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0); 
		}
		NUT_CORE_ASSERT(data, "Failed to load image! Which is from : {0}", path);

		m_Width = width;
		m_Height = height;																// gl func need unsigned int data So we assign the value of width to m_Width

		GLenum internalFormat = 0, dataFormat = 0;										// 数据内部储存格式 和 数据的上传格式
		if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}else if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		NUT_CORE_ASSERT((internalFormat & dataFormat), "Format not supported!")
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);				// 储存数据

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);				//纹理过滤（根据情况修改）
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);					//纹理环绕模式
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);		// 上传数据
		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		NUT_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)														// 数据和数据内存大小
	{
		NUT_PROFILE_FUNCTION();

		uint32_t bpp = (m_DataFormat == GL_RGBA ? 4 : 3);
		NUT_CORE_ASSERT((size == m_Width * m_Height * bpp), "Data must contain the full texture! Please check that the size of the data matches the format of the data");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);		// 上传数据
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		NUT_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}

}