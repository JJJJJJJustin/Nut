#include "nutpch.h"
#include "Nut/Renderer/RendererCommand.h"


namespace Nut
{

	Scope<RendererAPI> RendererCommand::s_RendererAPI = RendererAPI::Create();
	
	//RendererAPI* RendererCommand::s_RendererAPI = new OpenGLRendererAPI;


}