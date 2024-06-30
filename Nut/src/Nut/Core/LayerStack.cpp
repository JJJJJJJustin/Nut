#include "nutpch.h"
#include "LayerStack.h"

namespace Nut {
	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()								//���ٲ�ջ
	{
		for each (Layer* layer in m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);			//��ǰ������(emplace)
		m_LayerInsertIndex++;
		//layer->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)										//ɾ��ջ��Ԫ��
	{
		auto iter = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);			//std::find����һ�����������������Ϊ��һ��ָ��
		if (iter != m_Layers.begin() + m_LayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(iter);
			m_LayerInsertIndex--;												//emplace�����Ԫ�أ�Insert��ָ�����µ�Ԫ�أ�ջ��Ԫ�أ�����ɾ����Insert����һλ
		}
	}

	void LayerStack::PushOverLay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);											//��ǰ���壨ֱ�Ӳ���ĩβ��
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