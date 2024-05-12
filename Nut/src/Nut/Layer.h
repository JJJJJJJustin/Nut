#pragma once

#include "Core.h"
#include "Nut/Core/Timestep.h"
#include "Events/Event.h"

namespace Nut
{
	class NUT_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}			//此虚函数初步为空
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep& ts) {}
		virtual void OnEvent(Event& event) {}
		virtual void OnImGuiRender(){}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}