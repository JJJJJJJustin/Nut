#include "nutpch.h"
#include "LayerStack.h"

namespace Nut {
	LayerStack::LayerStack()
	{
		m_LayerInsert = m_Layers.begin();			//初始化插入指针的位置
	}

	LayerStack::~LayerStack()					//销毁层栈
	{
		for each (Layer * layer in m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer);			//从后向前插入(emplace)
	}

	void LayerStack::PopLayer(Layer* layer)						//删除栈顶元素
	{
		auto iter = std::find(m_Layers.begin(), m_Layers.end(), layer);	//std::find返回一个迭代器，可以理解为是一个指针
		if (iter != m_Layers.end())
		{
			m_Layers.erase(iter);
			m_LayerInsert--;											//emplace推入的元素，Insert会指向最新的元素（栈顶元素），故删除后将Insert后移一位
		}
	}

	void LayerStack::PushOverLay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);									//从前向后插（直接插在末尾）
	}

	void LayerStack::PopOverLay(Layer* overlay)
	{
		auto iter = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (iter != m_Layers.end())
		{
			m_Layers.erase(iter);
		}
	}

}