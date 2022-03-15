#ifndef QUEUE_H
#define QUEUE_H
#include  "deque.h"

namespace queueSTL{
	template<class T,class container=deque<T>>
	class queue{
	protected:
		container ct;//����deque
	public:
		typedef typename container::value_type value_type;//������������
		typedef typename container::size_type size_type;//���ȶ���
		typedef typename container::reference reference;//�������Ͷ���
		typedef typename container::const_reference const_reference;
	public:
		void push(const value_type &val) { ct.push_back(val); }//���
		void pop() { ct.pop_front(); }//����
		reference front() { return ct.front(); }//����ͷ��Ԫ��
		reference back() { return ct.back(); }//����β��Ԫ��
		bool empty() const { return ct.empty(); }//�п�
		size_type size()const { return ct.size(); }//��ȡ����
		friend bool operator==(const queue&rqu, const queue&lqu) { return rqu.ct == lqu.ct; }//����==
		friend bool operator<(const queue&rqu,const queue&lqu){return rqu.ct<lqu.ct}//����<

	};

}





#endif
