#include "Nut.h"
#include <Nut/Core/EntryPoint.h>

#include "EditorLayer.h"


namespace Nut {
	class NutEditor : public Application
	{
	public:
		NutEditor()
			:Application("Nut Editor")
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