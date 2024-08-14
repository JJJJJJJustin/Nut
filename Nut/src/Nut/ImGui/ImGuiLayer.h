#pragma once

#include "Nut/Core/Layer.h"

#include "Nut/Events/ApplicationEvent.h"
#include "Nut/Events/KeyEvent.h"
#include "Nut/Events/MouseEvent.h"

namespace Nut
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
	private:
		float m_Time = 0.0f;
		bool m_BlockEvents = true;
	};
}