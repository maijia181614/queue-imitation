#ifndef QUEUE_H
#define QUEUE_H
#include  "deque.h"

namespace queueSTL{
	template<class T,class container=deque<T>>
	class queue{
	protected:
		container ct;//引入deque
	public:
		typedef typename container::value_type value_type;//定义数据类型
		typedef typename container::size_type size_type;//长度定义
		typedef typename container::reference reference;//引用类型定义
		typedef typename container::const_reference const_reference;
	public:
		void push(const value_type &val) { ct.push_back(val); }//入队
		void pop() { ct.pop_front(); }//出队
		reference front() { return ct.front(); }//返回头部元素
		reference back() { return ct.back(); }//返回尾部元素
		bool empty() const { return ct.empty(); }//判空
		size_type size()const { return ct.size(); }//获取容量
		friend bool operator==(const queue&rqu, const queue&lqu) { return rqu.ct == lqu.ct; }//重载==
		friend bool operator<(const queue&rqu,const queue&lqu){return rqu.ct<lqu.ct}//重载<

	};

}





#endif
