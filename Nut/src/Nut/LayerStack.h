#pragma once

#include "Core.h"
#include "Layer.h"

#include <vector>

namespace Nut
{
	class NUT_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);				//普通图层推入
		void PopLayer(Layer* layer);				//普通图层移除
		void PushOverLay(Layer* overlay);			//覆盖图层处理
		void PopOverLay(Layer* overlay);			//覆盖图层移除

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }		//设计原因：当这个LayerStack对象作为for each loop（基于范围的for循环）的范围时
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }			//要求LayerStack 这个容器具有 begin() 和 end() 成员函数


	private:
		std::vector<Layer*> m_Layers;				//存入指向Layer对象的指针
		std::vector<Layer*>::iterator m_LayerInsert;
	};

}