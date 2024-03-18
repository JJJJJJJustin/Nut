#include "nutpch.h"
#include "LayerStack.h"

namespace Nut {
	LayerStack::LayerStack()
	{
		m_LayerInsert = m_Layers.begin();			//��ʼ������ָ���λ��
	}

	LayerStack::~LayerStack()					//���ٲ�ջ
	{
		for each (Layer * layer in m_Layers)
		{
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer);			//�Ӻ���ǰ����(emplace)
	}

	void LayerStack::PopLayer(Layer* layer)						//ɾ��ջ��Ԫ��
	{
		auto iter = std::find(m_Layers.begin(), m_Layers.end(), layer);	//std::find����һ�����������������Ϊ��һ��ָ��
		if (iter != m_Layers.end())
		{
			m_Layers.erase(iter);
			m_LayerInsert--;											//emplace�����Ԫ�أ�Insert��ָ�����µ�Ԫ�أ�ջ��Ԫ�أ�����ɾ����Insert����һλ
		}
	}

	void LayerStack::PushOverLay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);									//��ǰ���壨ֱ�Ӳ���ĩβ��
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