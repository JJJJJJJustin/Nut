#include "nutpch.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Nut
{
	#pragma region ������Ϣ�ص�����(OpenGLMessageCallback)����
	void OpenGLMessageCallback
	(unsigned source, unsigned type, unsigned id, unsigned severity, int length, const char* message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         NUT_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       NUT_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          NUT_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: NUT_CORE_TRACE(message); return;
		}

		NUT_CORE_ASSERT(false, "Unknown severity level!");
	}
	# pragma endregion

	void OpenGLRendererAPI::Init()
	{
		NUT_PROFILE_FUNCTION();

		#ifdef NUT_DEBUG
			//����OpenGL���ɵĵ�����Ϣ�����ɺ͹���
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);													//��Ⱦ��֮�󣬽��ǰ��������Ӱ�쵽���������������Ⱦ
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Cherno do:
	//void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount /*= 0*/){
	//	uint32_t count = indexCount ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
	//	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);}



}