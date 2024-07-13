#pragma once

#include "Nut/Core/Core.h"
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


	private:
		std::vector<Layer*> m_Layers;				//����ָ��Layer�����ָ��
		unsigned int m_LayerInsertIndex = 0;
	};

}