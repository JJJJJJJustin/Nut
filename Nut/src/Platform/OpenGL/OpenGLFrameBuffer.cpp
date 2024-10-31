#include "nutpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

// -------------------------------------------------------------------------------------------
// ------------------------------------ Utils ------------------------------------------------
// -------------------------------------------------------------------------------------------
namespace Utils 
{
	static GLenum TextureTarget(bool multisampled)
	{
		return multisampled == true ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static bool IsDepthFormat(Nut::FrameBufferAttachmentFormat format) 
	{
		switch(format)
		{
			case Nut::FrameBufferAttachmentFormat::DEPTH24STENCIL8 : return true;
		}
		return false;
	}

	static void CreateTextures(bool multisampled, uint32_t count, uint32_t* id)
	{
		glCreateTextures(TextureTarget(multisampled), count, id);
	}

	static void BindTexture(bool multisampled, uint32_t id)
	{
		glBindTexture(TextureTarget(multisampled), id);
	}

	static void AttachColorTexture(uint32_t id, int samples, GLenum format, uint32_t width, uint32_t height, int index) 
	{
		bool multisampled = samples > 1;
		if(multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else 
		{
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
	}

	static void AttachBufferTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else
		{
			glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			// ----- Or you can use renderbuffer object to attach depth/stencil attachment ----
			//glCreateRenderbuffers(1, &id);
			//glBindRenderbuffer(GL_RENDERBUFFER, id);
			//glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
			//glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
	}
}

// -------------------------------------------------------------------------------------------
// -------------------------------------- Nut ------------------------------------------------
// -------------------------------------------------------------------------------------------
namespace Nut
{
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		:m_Specification(spec)
	{
		for(auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec)) 
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else 
				m_BufferAttachmentSpecification = spec;
		}

		Recreate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachmentIDs.size(), m_ColorAttachmentIDs.data());
		glDeleteRenderbuffers(1, &m_BufferAttachmentID);
	}

	void OpenGLFrameBuffer::Recreate()
	{
		if (m_RendererID) 
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachmentIDs.size(), m_ColorAttachmentIDs.data());
			glDeleteRenderbuffers(1, &m_BufferAttachmentID);

			m_ColorAttachmentIDs.clear();
			m_BufferAttachmentID = 0;
		}

		// --------- Create Framebuffer Object ---------------
		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		// --------- Create Texture Attachment ---------------
		bool multisample = m_Specification.Samples > 1;

		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachmentIDs.resize(m_ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multisample, m_ColorAttachmentIDs.size(), m_ColorAttachmentIDs.data());
			for (size_t i = 0; i < m_ColorAttachmentIDs.size(); i++)
			{
				Utils::BindTexture(multisample, m_ColorAttachmentIDs[i]);
				switch (m_ColorAttachmentSpecifications[i])
				{
				case FrameBufferAttachmentFormat::RGBA8:
					Utils::AttachColorTexture(m_ColorAttachmentIDs[i], m_Specification.Samples, GL_RGBA8, m_Specification.Width, m_Specification.Height, i);
					break;
				}
			}
		}
		// --------- Create Renderbuffer Object --------------
		if (m_BufferAttachmentSpecification != FrameBufferAttachmentFormat::None)
		{
			Utils::CreateTextures(multisample, 1, &m_BufferAttachmentID);
			Utils::BindTexture(multisample, m_BufferAttachmentID);
			switch (m_BufferAttachmentSpecification)
			{
			case FrameBufferAttachmentFormat::DEPTH24STENCIL8:
				Utils::AttachBufferTexture(m_BufferAttachmentID, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			}
		}
		// ---------- Mapping the fragment shader's output variables to different color attachments ------------
		if (m_ColorAttachmentIDs.size() > 1)
		{
			NUT_CORE_ASSERT((m_ColorAttachmentIDs.size() <= 4), "");

			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachmentIDs.size(), buffers);
		}
		else if (m_ColorAttachmentIDs.empty())
		{
			// Only depth pass
			glDrawBuffer(GL_NONE);
		}
		// --------- Check Framebuffer correctness -----------
		NUT_CORE_ASSERT((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "Framebuffer is not complete");
		// --------- Unbind Framebuffer ----------------------
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);							// 及时为渲染结果更新视口
	}

	void OpenGLFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;

		Recreate();
	}
}