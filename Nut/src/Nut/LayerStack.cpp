#include "nutpch.h"
#include "LayerStack.h"

namespace Nut {
	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()								//���ٲ�ջ
	{
		for each (Layer * layer in m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);			//��ǰ������(emplace)
		m_LayerInsertIndex++;
	}

	void LayerStack::PopLayer(Layer* layer)										//ɾ��ջ��Ԫ��
	{
		auto iter = std::find(m_Layers.begin(), m_Layers.end(), layer);			//std::find����һ�����������������Ϊ��һ��ָ��
		if (iter != m_Layers.end())
		{
			m_Layers.erase(iter);
			m_LayerInsertIndex--;								//emplace�����Ԫ�أ�Insert��ָ�����µ�Ԫ�أ�ջ��Ԫ�أ�����ɾ����Insert����һλ
		}
	}

	void LayerStack::PushOverLay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);							//��ǰ���壨ֱ�Ӳ���ĩβ��
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