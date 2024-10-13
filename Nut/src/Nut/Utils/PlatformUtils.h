#pragma once

#include <string>

namespace Nut 
{
	class FileDialogs
	{
	public:
		// TODO: This two functions needs to return empty string if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}