#include "nutpch.h"
#include "LayerStack.h"

namespace Nut {
	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()								//销毁层栈
	{
		for each (Layer* layer in m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);			//从前向后插入(emplace)
		m_LayerInsertIndex++;
		//layer->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)										//删除栈顶元素
	{
		auto iter = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);			//std::find返回一个迭代器，可以理解为是一个指针
		if (iter != m_Layers.begin() + m_LayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(iter);
			m_LayerInsertIndex--;												//emplace推入的元素，Insert会指向最新的元素（栈顶元素），故删除后将Insert后移一位
		}
	}

	void LayerStack::PushOverLay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);											//从前向后插（直接插在末尾）
		//overlay->OnAttach();
	}

	void LayerStack::PopOverLay(Layer* overlay)
	{
		auto iter = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
		if (iter != m_Layers.end())
		{
			overlay->OnDetach();
			m_Layers.erase(iter);
		}
	}

}