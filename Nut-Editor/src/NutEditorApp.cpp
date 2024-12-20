#include "Nut.h"
#include <Nut/Core/EntryPoint.h>

#include "EditorLayer.h"


namespace Nut {
	class NutEditor : public Application
	{
	public:
		NutEditor(ApplicationCommandLineArgs args)
			:Application("Nut Editor", args)
		{
			PushLayer(new EditorLayer());
		}

		~NutEditor()
		{
		}

	};


	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new NutEditor(args);
	}

}