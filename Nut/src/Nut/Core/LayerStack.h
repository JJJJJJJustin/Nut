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

		void PushLayer(Layer* layer);				//��ͨͼ������
		void PopLayer(Layer* layer);				//��ͨͼ���Ƴ�
		void PushOverLay(Layer* overlay);			//����ͼ�㴦��
		void PopOverLay(Layer* overlay);			//����ͼ���Ƴ�

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }		//���ԭ�򣺵����LayerStack������Ϊfor each loop�����ڷ�Χ��forѭ�����ķ�Χʱ
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }			//Ҫ��LayerStack ����������� begin() �� end() ��Ա����
		std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
		std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }

		std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend()   { return m_Layers.rend(); }
		std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rend()   const { return m_Layers.rbegin(); }

	private:
		std::vector<Layer*> m_Layers;				//����ָ��Layer�����ָ��
		unsigned int m_LayerInsertIndex = 0;
	};

}