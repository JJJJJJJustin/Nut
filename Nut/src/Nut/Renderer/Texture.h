#pragma once

#include <string>

#include "Nut/Core/Core.h"

namespace Nut
{

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;
		
		virtual void Bind(uint32_t slot = 0) const = 0;
	};



	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);								//手动设置纹理大小（宽高），并通过 SetData 设置纯色纹理。
		static Ref<Texture2D> Create(const std::string& path);										//根据图像文件读取
	};

}