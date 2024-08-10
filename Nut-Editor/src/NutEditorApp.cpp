#include "Nut.h"
#include <Nut/Core/EntryPoint.h>

#include "EditorLayer.h"


namespace Nut {
	class NutEditor : public Application
	{
	public:
		NutEditor()
		{
			PushLayer(new EditorLayer());
		}

		~NutEditor()
		{
		}

	};


	Application* CreateApplication()
	{
		return new NutEditor();
	}

}