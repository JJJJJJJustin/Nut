#pragma once

namespace Nut
{
	// ---- An Interface for rendering context ----
	class GraphicsContext 
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	private:

	};

}