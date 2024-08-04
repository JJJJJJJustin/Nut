#pragma once

#include "Nut/Core/Base.h"
#include "Nut/Core/Layer.h"

#include <vector>

namespace Nut
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);				//普通图层推入
		void PopLayer(Layer* layer);				//普通图层移除
		void PushOverLay(Layer* overlay);			//覆盖图层处理
		void PopOverLay(Layer* overlay);			//覆盖图层移除

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }		//设计原因：当这个LayerStack对象作为for each loop（基于范围的for循环）的范围时
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }			//要求LayerStack 这个容器具有 begin() 和 end() 成员函数
		std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
		std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }

		std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend()   { return m_Layers.rend(); }
		std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rend()   const { return m_Layers.rbegin(); }

	private:
		std::vector<Layer*> m_Layers;				//存入指向Layer对象的指针
		unsigned int m_LayerInsertIndex = 0;
	};

}