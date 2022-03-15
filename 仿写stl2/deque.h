
#pragma once
#ifndef DEQUE
#define DEQUE
#include<initializer_list>//初始化列表
#include"Algobase.h"//基础算法库
#include<algorithm>
#include "Alloc.h"//内存分配
#include"Iterator1.h"//引入迭代器库
#include"Construct.h"//对象构造库
using std::uninitialized_fill;
using std::uninitialized_copy;
namespace queueSTL {
	//分配缓冲区中的元素个数，如果有指定个数n,直接返回n
	//如果不是n,则采用默认计算512/sz计算缓冲区中的元素个数，sz为元素大小
	size_t _deque_buf_size(size_t n, size_t sz) {
		return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
	}


	template<class T, class Ref, class Ptr, size_t Bufsize>
	struct _deque_iterator {//deque专用迭代器
		typedef _deque_iterator<T, T&, T*, Bufsize> iterator;//deque迭代器定义
		typedef _deque_iterator<T, const T&, const T*, Bufsize> const_iterator;
		static size_t buffer_size() { return _deque_buf_size(Bufsize, sizeof(T)); }//计算缓冲区元素个数

		typedef random_access_iterator_tag iterator_category;//迭代器类型
		typedef T value_type;//存数据类型
		typedef Ptr pointer;//指针
		typedef Ref reference;//引用类型
		typedef size_t size_type;//数量
		typedef ptrdiff_t difference_type;//迭代器距离类型
		typedef T** map_pointer;//map的指针(map存的也是指针，指向缓冲区)
		typedef _deque_iterator self;

		T *curr;//指向缓冲区指针所在的地方
		T *first;//缓冲区的头
		T *last;//缓冲区尾部
		map_pointer node;//控制中心map

		_deque_iterator(T *f, map_pointer n) :curr(f), first(*n), last(*n + buffer_size()), node(n) {}//初始化1
		_deque_iterator() :curr(0), first(0), last(0), node(0) {}//初始化2
		_deque_iterator(const iterator &rhs) :curr(rhs.curr), first(rhs.first), last(rhs.last), node(rhs.node) {}//初始化3
		reference operator*() { return *curr; }//重载*操作符 返回现缓冲区内的指针
		pointer operator->() { return &(operator*()); }//重载->操作符 返回引用

		difference_type operator -(const _deque_iterator<T, Ref, Ptr, Bufsize>&x)const {//重载减，计算迭代器之间的元素个数，可跨缓冲区
			return difference_type(buffer_size())*(node - x.node - 1) + (curr - first) + (x.last - x.curr);
		}

		void set_node(map_pointer new_node) {//设置一个新的缓冲区
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}

		self& operator++() {//重载前缀加
			++curr;
			if (curr == last) {
				set_node(node + 1);
				curr = first;
			}
			return *this;
		}

		self operator++(int) {//重载后缀加 返回的是临时对象
			self tmp = *this;
			++*this;
			return tmp;
		}

		self& operator--() {//重载前缀减
			if (curr == first) {
				set_node(node - 1);
				curr = last;
			}
			--curr;
			return *this;
		}

		self operator--(int) {//重载后缀减
			self tmp = *this;
			--*this;
			return tmp;
		}

		self& operator+=(difference_type n) {//重载+=
			difference_type offset = n + (curr - first);
			if (offset >= 0 && offset < difference_type(buffer_size())) {//判断自身位置+n 不跨缓冲区+  直接移动curr
				curr += n;
			}
			else {//跨越缓冲区  计算跨越缓冲区个数 设置新的缓冲区 计算新的curr
				difference_type node_offset = offset > 0 ? (offset / (difference_type(buffer_size()))) : -difference_type((-offset - 1) / buffer_size()) - 1;
				set_node(node + node_offset);
				curr = first + (offset - node_offset*difference_type(buffer_size()));//总跨越数-跨越过的缓冲区里元素个数
			}
			return *this;
		}

		self operator+(difference_type n)const {//重载有参加
			self tmp = *this;
			return tmp += n;
		}

		self& operator-=(difference_type n) {//重载-=
			return *this += -n;
		}

		self operator-(difference_type n)const {//重载减
			self tmp = *this;
			return tmp -= n;
		}

		reference operator[](difference_type n)const { return *(*this + n); }//重载[]实现连续随机访问
		bool operator==(const self &rhs)const { return curr == rhs.curr; }//重载==
		bool operator!=(const self &rhs)const { return !(*this == rhs); }//重载!=
		bool operator<(const self &rhs)const {//重载<号  在同一缓冲区比较curr位置，若不是则比较缓冲区位置
			return node == rhs.node ? curr < rhs.curr : (node < rhs.node);
		}
	};

	template<class T, class Alloc = alloc, size_t Bufsize = 0>
	class deque {
	public:
		typedef T value_type;//数据类型
		typedef value_type* pointer;//指针
		typedef const value_type* const_pointer;//常量指针
		typedef value_type& reference;//引用类型
		typedef const value_type& const_reference;
		typedef size_t size_type;//数据容量
		typedef ptrdiff_t difference_type;
		typedef _deque_iterator<T, T&, T*, Bufsize> iterator;//迭代器定义
		typedef _deque_iterator<T, const T&, const T*, Bufsize>const_iterator;
	protected://一些敏感数据
		typedef pointer* map_pointer;//控制中心指针
		typedef simple_alloc<pointer, Alloc> map_alloctor;//控制中心分配器
		typedef simple_alloc<value_type, Alloc>data_allocator;//数据分配器
		static size_type buffer_size() {//缓冲区大小
			return _deque_buf_size(Bufsize, sizeof(value_type));
		}
		static size_type initial_map_size() { return 8; }//初始化控制中心长度
		iterator start;//控制中心头部迭代器
		iterator finish;//控制中心尾部迭代器
		map_pointer map;//控制中心指针
		size_type map_size;//控制中心长度

	public:
		iterator begin() { return start; }//返回头部迭代器
		const_iterator begin()const { return start; }//返回头部迭代器
		const_iterator cbegin()const { return start; }//返回头部迭代器
		iterator end() { return finish; }//返回尾部迭代器
		const_iterator end()const { return finish; }//返回尾部迭代器
		const_iterator cend()const { return finish; }//返回尾部迭代器
		reference operator[](size_type n) { return start[difference_type(n)]; }//重载[]号，直接调用迭代器[]号进行连续随机访问
		const_reference operator[](size_type n)const { return start[difference_type(n)]; }//重载[]
		reference front() { return *start; }//返回头部迭代器指针
		const_reference front()const { return *start; }//返回头部迭代器指针

		reference back() {//返回尾部迭代器指针
			iterator tmp = finish;
			--tmp;
			return *tmp;
		}
		const_reference back()const {//返回尾部迭代器指针
			const_iterator tmp = finish;
			--tmp;
			return *tmp;
		}

		size_type size()const { return finish - start; }//返回容器中元素个数
		size_type max_size()const { return size_type(-1); }//返回最大空间
		bool empty()const { return start == finish; }//判空

	public:
		deque() :start(), finish(), map(0), map_size(0) {//无参构造
			create_map_and_nodes(0);//
		}

		deque(const deque &rhs) :start(), finish(), map(0), map_size(0) {//传入队列进行复制初始化
			create_map_and_nodes(rhs.size());
			try {
				uninitialized_copy(rhs.begin(), rhs.end, start);//复制
			}
			catch (...) {
				destroy_map_and_nodes();
				throw;
			}
		}

		deque(size_type n, const value_type &val) :start(), finish(), map(0), map_size(0) {//构造函数
			fill_initialize(n, val);
		}
		deque(int n, const value_type &val) :start(), finish(), map(0), map_size(0) {
			fill_initialize(n, val);
		}
		deque(long n, const value_type &val) :start(), finish(), map(0), map_size(0) {
			fill_initialize(n, val);
		}
		template<class InputIterator>
		deque(InputIterator first, InputIterator last) : start(), finish(), map(0), map_size(0) {//传入迭代器构造函数
			range_initialize(first, last, iterator_category(first))
		}
		deque(std::initializer_list<value_type> il) {//使用列表进行初始化，需指定迭代器类型
			range_initialize(il.begin(), il.end, input_iterator_tag())
		}

		~deque() {//析构函数
			destroy(start, finish);
			destroy_map_and_nodes();
		}

	public:
		void push_back(const value_type &val) {//队尾入队
			if (finish.curr != finish.last - 1) {//查看容量
				construct(finish.curr, val);
				++finish.curr;
			}
			else {
				push_back_aux(val);//容量不够时，扩充再入队
			}
		}

		void push_front(const value_type &val) {//队头入队
			if (start.curr != start.first) {//查看容量
				construct(start.curr - 1, val);
				--start.curr;
			}
			else {
				push_front_aux(val);//容量不够时，扩充再入队
			}
		}

		void pop_back() {
			if (finish.curr != finish.first) {//队尾出队
				--finish.curr;
				destroy(finish.curr);
			}
			else {
				pop_back_aux();//可能要释放缓冲区
			}
		}

		void pop_front() {//队头出队
			if (start.curr + 1 != start.last) {
				destroy(start.curr);
				++start.curr;
			}
			else {
				pop_front_aux();
			}

		}
	public:
		iterator insert(iterator pos, const value_type &val) {//队中插入
			if (pos.curr == start.curr) {//在队头直接插入
				push_front(val);
				return start;
			}
			else if (pos.curr == finish.curr) {//在队尾直接插入,返回迭代器
				push_back(val);
				iterator tmp = finish;
				--tmp;
				return tmp;
			}
			else {
				return insert_aux(pos, val);//在队中插入
			}
		}

	protected:
		iterator insert_aux(iterator pos, const value_type &val);
	protected:
		void push_back_aux(const value_type&);//队尾入队基础
		void push_front_aux(const value_type&);//队头入队基础
		void pop_back_aux();//队尾弹出基础
		void pop_front_aux();//队头弹出基础
	public:
		iterator erase(iterator pos);//移除迭代器指向序列
		iterator erase(iterator first, iterator last);//移除迭代器范围内的元素
		void clear();//清除所有内容
	protected:
		void create_map_and_nodes(size_type num_elements);
		void destroy_map_and_nodes();
		void fill_initialize(size_type n, const value_type&);
		template<class InputIterator>
		void range_initialize(InputIterator first, InputIterator last, input_iterator_tag);
		template<class ForwardIterator>
		void range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag);



	protected:
		pointer allocate_node() { return data_allocator::allocate(buffer_size()); }//为缓冲区分配内存
		void deallocate_node(pointer ptr) {//为缓冲区释放内存
			return data_allocator::deallocate(ptr, buffer_size());
		}
		void reallocate_map_front(size_type nodes_to_add = 1) {//判断map满了没，为队头插入调用
			if (nodes_to_add + map > start.node) {
				reallocate_map(nodes_to_add, true);
			}
		}
		void reallocate_map_back(size_type nodes_to_add = 1) {
			if (nodes_to_add + 1 > map_size-(finish.node - map)) {//判断map满了没，为队尾插入调用
				reallocate_map(nodes_to_add, false);
			}
		}
		void reallocate_map(size_type nodes_to_add, bool add_front);//调整map空间
	};

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::create_map_and_nodes(size_type num_elements) {//创建控制中心和分配缓冲区内存
		size_type num_nodes = num_elements / buffer_size() + 1;//计算创建缓冲区个数
		map_size = max(initial_map_size(), num_nodes + 2);//与默认值比较，取大的
		map = map_alloctor::allocate(map_size);//创建控制中心
		map_pointer nstart = map + (map_size - num_nodes) / 2;//使控制中心开头指针在中间位置
		map_pointer nfinish = nstart + num_nodes - 1;//控制中心尾指针
		map_pointer curr;//控制中心目前指针
		try {
			for (curr = nstart; curr <= nfinish; ++curr) {//循环分配缓冲区的内存
				*curr = allocate_node();
			}
		}
		catch (...) {//异常释放内存
			for (map_pointer n = nstart; n < curr; ++n) {
				deallocate_node(*n);
			}
			map_alloctor::deallocate(map, map_size);
			throw;
		}
		start.set_node(nstart);//设置控制中心头指针
		finish.set_node(nfinish);//设置控制中心尾指针
		start.curr = start.first;//头迭代器的目前指针
		finish.curr = finish.first + num_elements % (buffer_size());//尾迭代器的目前指针
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::destroy_map_and_nodes() {//销毁控制中心和释放缓冲区内存
		for (map_pointer curr = start.node; curr <= finish.node; ++curr) {
			deallocate_node(*curr);
		}
		map_alloctor::deallocate(map, map_size);
	}

	template<class T, class Alloc, size_t Bufsize>
	typename deque<T, Alloc, Bufsize>::iterator
		deque<T, Alloc, Bufsize>::erase(iterator pos) {//移除迭代器指向的元素
		iterator next = pos;
		++next;
		difference_type index = pos - start;
		if (index < (size() >> 1)) {//若前面元素较少,则处理前面
			copy_backward(start, pos, next);//把start到pos除了pos元素全部copy到next之前，从后面开始
			pop_front();//弹出队头多余的第一个元素
		}
		else {//处理后面比较方便
			copy(next, finish, pos);//把next到finish copy到pos开始到最后，pos覆盖
			pop_back();//弹出队尾多余的元素
		}
		return start + index;
	}

	template<class T, class Alloc, size_t Bufsize>
	typename deque<T, Alloc, Bufsize>::iterator
		deque<T, Alloc, Bufsize>::erase(iterator first, iterator last) {//移除迭代器范围内的元素
		if (first == start&&finifsh == last) {//范围刚好是控制中心头到尾，全部清除
			clear();
			return finish;
		}
		else {
			difference_type n = last - first;//区间元素
			difference_type elems_before = first - start;//前面的元素数
			if (elems_before < (size() - n) / 2) {//若前面的元素较少
				copy_backward(start, first, last);//把start到first除了first元素全部copy到last之前，从后面开始
				iterator new_start = start + n;
				destroy(start, new_start);//析构范围内的对象
				for (map_pointer curr_node = start.node; curr_node < new_start.node; ++cur_node) {//释放范围内存
					data_allocator::deallocate(*curr_node, buffer_size());
				}
				start = new_start;//修正头迭代器
			}
			else {//若前面的元素较多
				copy(last, finish, first);//把last到finish除了finish全部元素从first开始copy覆盖
				iterator new_finish = finish - n;//
				destroy(new_finish, finish);//先析构范围内对象
				for (map_pointer cur_node = new_finish.node + 1; cur <= finish.node; ++cur_node) {
					data_allocator::deallocate(*cur_node, buffer_size());//释放范围内存
				}
				finish = new_finish;//修正尾迭代器
			}
			return start + elems_before;//第一种情况返回到last的位置迭代器，第二种情况返回到first位置的迭代器
		}
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::clear() {//清除整个deque,但是会保留头部的一个缓冲区
		for (map_pointer cur_node = start.node + 1; cur_node < finish.node; ++cur_node) {
			destroy(*cur_node, *cur_node + buffer_size());//析构每个缓冲区内所有元素
			data_allocator::deallocate(*cur_node, buffer_size());//释放每个缓冲区内内存
		}
		if (start.node != finish.node) {//头尾不是同一个迭代器，各自指向的区域再清除
			destroy(start.curr, start.last);
			destroy(finish.first, finish.curr);
			data_allocator::deallocate(finish.first, buffer_size());
		}
		else {
			destroy(start.curr, start.last);//头迭代器缓冲区内析构
		}
		finish = start;//头尾调整
	}





	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::fill_initialize(size_type n, const value_type &val) {//批量初始化
		create_map_and_nodes(n);
		map_pointer curr;
		try {
			for (curr = start.node; curr < finish.node; ++curr) {//初始化缓冲区内空间
				uninitialized_fill(*curr, *curr + buffer_size(), val);
			}
			uninitialized_fill(finish.first, finish.curr, val);
		}
		catch (...) {//异常则销毁
			for (map_pointer ptr = start.node; ptr < curr; ++ptr) {
				destroy(*ptr, *ptr + buffer_size());
			}
			destroy_map_and_nodes();
		}
	}


	template<class T, class Alloc, size_t Bufsize>
	template<class ForwardIterator>
	void deque<T, Alloc, Bufsize>::range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {//范围初始化
		size_type n = 0;
		__distance(first, last, n);//计算两个迭代器之间的距离
		create_map_and_nodes(n);//创建控制中心和分配缓冲区内存
		try {
			uninitialized_copy(first, last, start);//批量复制初始化
		}
		catch (...) {
			destroy_map_and_nodes();
		}
	}

	template<class T, class Alloc, size_t Bufsize>//到这
	template<class InputIterator>
	void deque<T, Alloc, Bufsize>::range_initialize(InputIterator first, InputIterator last, input_iterator_tag) {//范围初始化
		create_map_and_nodes(0);
		for (; first != last; ++first) {
			push_back(*first);//循环入队
		}
	}


	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::reallocate_map(size_type nodes_to_add, bool add_front) {//重新配置map
		size_type old_num_nodes = finish.node - start.node + 1;//原缓冲区数量
		size_type new_num_nodes = old_num_nodes + nodes_to_add;//新缓冲区数量
		map_pointer new_start;
		if (map_size > 2 * new_num_nodes) {//空间够用了
			new_start = map + (map_size - new_num_nodes) / 2 + (add_front ? nodes_to_add : 0);//map新的头指针
			if (new_start < start.node) {//新的头指针在start左边
				copy(start.node, finish.node + 1, new_start);//把start到finish从新的头指针开始copy
			}
			else {
				copy_backward(start.node, finish.node + 1, new_start + old_num_nodes);//把start到finish copy到new_start + old_num_nodes,从尾开始
			}
		}
		else {//空间不够用了
			size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;//计算新map的大小
			map_pointer new_map = map_alloctor::allocate(new_map_size);//分配内存
			new_start = new_map + (new_map_size - new_num_nodes) / 2 + (add_front ? nodes_to_add : 0);//新的头指针
			copy(start.node, finish.node + 1, new_start);//把start到finish从new_start开始copy
			map_alloctor::deallocate(map, map_size);//释放旧map内存
			map = new_map;
			map_size = new_map_size;
		}
		start.set_node(new_start);//重设start
		finish.set_node(start.node + old_num_nodes - 1);//重设finish
	}

	template<class T, class Alloc, size_t Bufsize>
	typename deque<T, Alloc, Bufsize>::iterator
		deque<T, Alloc, Bufsize>::insert_aux(iterator pos, const value_type &val) {
		difference_type index = pos - start;//插入点前面元素个数
		value_type val_copy = val;
		if (index < size() / 2) {//前面元素比较少
			push_front(front());//先在队头重复加入第一个元素
			iterator front1 = start;
			++front1;//start+1 原队头
			iterator front2 = front1;
			++front2;//原队头+1 是第二个元素
			pos = start + index;
			iterator pos1 = pos;
			++pos1;//pos+1 原要复制那一段的尾
			copy(front1, pos1, front2);//把front1到pos1从front开始copy
		}
		else {//如果插入点后面元素少
			push_back(back());//先在队尾重复加入最后一个元素
			iterator back1 = finish;
			--back1;//原队尾
			iterator back2 = back1;
			--back2;//原队尾-1
			pos = start + index;
			copy_backward(pos, back2, back1);//把pos到back2从back1开始copy  从尾部开始
		}
		*pos = val_copy;//在插入点设置新值
		return pos;
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::push_back_aux(const value_type&val) {//最后一个缓冲区只剩一个元素空间，就是空间不足时调用
		value_type val_copy = val;//传入值
		reallocate_map_back();//判断重新配置map
		*(finish.node + 1) = allocate_node();//重新配置一个新缓冲区的内存
		try {
			construct(finish.curr, val_copy);//设置新值构造对象
			finish.set_node(finish.node + 1);//给尾迭代器设置新node
			finish.curr = finish.first;//设置新尾迭代器
		}
		catch (...) {
			deallocate_node(*(finish.node + 1));
			throw;
		}
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::push_front_aux(const value_type &val) {//最后一个缓冲区只剩一个元素空间，就是空间不足时调用
		value_type val_copy = val;//传入值
		reallocate_map_front();//判断要不要配置新的map
		*(start.node - 1) = allocate_node();//重新配置一个新缓冲区的内存 在头
		try {
			start.set_node(start.node - 1);//给头迭代器设置新node
			start.curr = start.last - 1;//curr设置成新缓冲区的尾
			construct(start.curr, val);//给插入的值分配内存构造对象
		}
		catch (...) {
			start.set_node(start.node + 1);
			start.curr = start.first;
			deallocate_node(*(start.node - 1));
			throw;
		}
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::pop_back_aux() {//最后一个缓冲区啥都没
		deallocate_node(finish.first);//释放最后缓冲区内存
		finish.set_node(finish.node - 1);//为finish设置为上一个node
		finish.curr = finish.last - 1;
		destroy(finish.curr);//析构元素
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::pop_front_aux() {//第一个缓冲区只有一个元素
		destroy(start.curr);//析构这个元素
		deallocate_node(start.first);//释放第一缓冲区内存
		start.set_node(start.node + 1);//为start设置新node
		start.curr = start.first;
	}

}


#endif