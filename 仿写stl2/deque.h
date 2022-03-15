
#pragma once
#ifndef DEQUE
#define DEQUE
#include<initializer_list>//��ʼ���б�
#include"Algobase.h"//�����㷨��
#include<algorithm>
#include "Alloc.h"//�ڴ����
#include"Iterator1.h"//�����������
#include"Construct.h"//�������
using std::uninitialized_fill;
using std::uninitialized_copy;
namespace queueSTL {
	//���仺�����е�Ԫ�ظ����������ָ������n,ֱ�ӷ���n
	//�������n,�����Ĭ�ϼ���512/sz���㻺�����е�Ԫ�ظ�����szΪԪ�ش�С
	size_t _deque_buf_size(size_t n, size_t sz) {
		return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
	}


	template<class T, class Ref, class Ptr, size_t Bufsize>
	struct _deque_iterator {//dequeר�õ�����
		typedef _deque_iterator<T, T&, T*, Bufsize> iterator;//deque����������
		typedef _deque_iterator<T, const T&, const T*, Bufsize> const_iterator;
		static size_t buffer_size() { return _deque_buf_size(Bufsize, sizeof(T)); }//���㻺����Ԫ�ظ���

		typedef random_access_iterator_tag iterator_category;//����������
		typedef T value_type;//����������
		typedef Ptr pointer;//ָ��
		typedef Ref reference;//��������
		typedef size_t size_type;//����
		typedef ptrdiff_t difference_type;//��������������
		typedef T** map_pointer;//map��ָ��(map���Ҳ��ָ�룬ָ�򻺳���)
		typedef _deque_iterator self;

		T *curr;//ָ�򻺳���ָ�����ڵĵط�
		T *first;//��������ͷ
		T *last;//������β��
		map_pointer node;//��������map

		_deque_iterator(T *f, map_pointer n) :curr(f), first(*n), last(*n + buffer_size()), node(n) {}//��ʼ��1
		_deque_iterator() :curr(0), first(0), last(0), node(0) {}//��ʼ��2
		_deque_iterator(const iterator &rhs) :curr(rhs.curr), first(rhs.first), last(rhs.last), node(rhs.node) {}//��ʼ��3
		reference operator*() { return *curr; }//����*������ �����ֻ������ڵ�ָ��
		pointer operator->() { return &(operator*()); }//����->������ ��������

		difference_type operator -(const _deque_iterator<T, Ref, Ptr, Bufsize>&x)const {//���ؼ������������֮���Ԫ�ظ������ɿ绺����
			return difference_type(buffer_size())*(node - x.node - 1) + (curr - first) + (x.last - x.curr);
		}

		void set_node(map_pointer new_node) {//����һ���µĻ�����
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}

		self& operator++() {//����ǰ׺��
			++curr;
			if (curr == last) {
				set_node(node + 1);
				curr = first;
			}
			return *this;
		}

		self operator++(int) {//���غ�׺�� ���ص�����ʱ����
			self tmp = *this;
			++*this;
			return tmp;
		}

		self& operator--() {//����ǰ׺��
			if (curr == first) {
				set_node(node - 1);
				curr = last;
			}
			--curr;
			return *this;
		}

		self operator--(int) {//���غ�׺��
			self tmp = *this;
			--*this;
			return tmp;
		}

		self& operator+=(difference_type n) {//����+=
			difference_type offset = n + (curr - first);
			if (offset >= 0 && offset < difference_type(buffer_size())) {//�ж�����λ��+n ���绺����+  ֱ���ƶ�curr
				curr += n;
			}
			else {//��Խ������  �����Խ���������� �����µĻ����� �����µ�curr
				difference_type node_offset = offset > 0 ? (offset / (difference_type(buffer_size()))) : -difference_type((-offset - 1) / buffer_size()) - 1;
				set_node(node + node_offset);
				curr = first + (offset - node_offset*difference_type(buffer_size()));//�ܿ�Խ��-��Խ���Ļ�������Ԫ�ظ���
			}
			return *this;
		}

		self operator+(difference_type n)const {//�����вμ�
			self tmp = *this;
			return tmp += n;
		}

		self& operator-=(difference_type n) {//����-=
			return *this += -n;
		}

		self operator-(difference_type n)const {//���ؼ�
			self tmp = *this;
			return tmp -= n;
		}

		reference operator[](difference_type n)const { return *(*this + n); }//����[]ʵ�������������
		bool operator==(const self &rhs)const { return curr == rhs.curr; }//����==
		bool operator!=(const self &rhs)const { return !(*this == rhs); }//����!=
		bool operator<(const self &rhs)const {//����<��  ��ͬһ�������Ƚ�currλ�ã���������Ƚϻ�����λ��
			return node == rhs.node ? curr < rhs.curr : (node < rhs.node);
		}
	};

	template<class T, class Alloc = alloc, size_t Bufsize = 0>
	class deque {
	public:
		typedef T value_type;//��������
		typedef value_type* pointer;//ָ��
		typedef const value_type* const_pointer;//����ָ��
		typedef value_type& reference;//��������
		typedef const value_type& const_reference;
		typedef size_t size_type;//��������
		typedef ptrdiff_t difference_type;
		typedef _deque_iterator<T, T&, T*, Bufsize> iterator;//����������
		typedef _deque_iterator<T, const T&, const T*, Bufsize>const_iterator;
	protected://һЩ��������
		typedef pointer* map_pointer;//��������ָ��
		typedef simple_alloc<pointer, Alloc> map_alloctor;//�������ķ�����
		typedef simple_alloc<value_type, Alloc>data_allocator;//���ݷ�����
		static size_type buffer_size() {//��������С
			return _deque_buf_size(Bufsize, sizeof(value_type));
		}
		static size_type initial_map_size() { return 8; }//��ʼ���������ĳ���
		iterator start;//��������ͷ��������
		iterator finish;//��������β��������
		map_pointer map;//��������ָ��
		size_type map_size;//�������ĳ���

	public:
		iterator begin() { return start; }//����ͷ��������
		const_iterator begin()const { return start; }//����ͷ��������
		const_iterator cbegin()const { return start; }//����ͷ��������
		iterator end() { return finish; }//����β��������
		const_iterator end()const { return finish; }//����β��������
		const_iterator cend()const { return finish; }//����β��������
		reference operator[](size_type n) { return start[difference_type(n)]; }//����[]�ţ�ֱ�ӵ��õ�����[]�Ž��������������
		const_reference operator[](size_type n)const { return start[difference_type(n)]; }//����[]
		reference front() { return *start; }//����ͷ��������ָ��
		const_reference front()const { return *start; }//����ͷ��������ָ��

		reference back() {//����β��������ָ��
			iterator tmp = finish;
			--tmp;
			return *tmp;
		}
		const_reference back()const {//����β��������ָ��
			const_iterator tmp = finish;
			--tmp;
			return *tmp;
		}

		size_type size()const { return finish - start; }//����������Ԫ�ظ���
		size_type max_size()const { return size_type(-1); }//�������ռ�
		bool empty()const { return start == finish; }//�п�

	public:
		deque() :start(), finish(), map(0), map_size(0) {//�޲ι���
			create_map_and_nodes(0);//
		}

		deque(const deque &rhs) :start(), finish(), map(0), map_size(0) {//������н��и��Ƴ�ʼ��
			create_map_and_nodes(rhs.size());
			try {
				uninitialized_copy(rhs.begin(), rhs.end, start);//����
			}
			catch (...) {
				destroy_map_and_nodes();
				throw;
			}
		}

		deque(size_type n, const value_type &val) :start(), finish(), map(0), map_size(0) {//���캯��
			fill_initialize(n, val);
		}
		deque(int n, const value_type &val) :start(), finish(), map(0), map_size(0) {
			fill_initialize(n, val);
		}
		deque(long n, const value_type &val) :start(), finish(), map(0), map_size(0) {
			fill_initialize(n, val);
		}
		template<class InputIterator>
		deque(InputIterator first, InputIterator last) : start(), finish(), map(0), map_size(0) {//������������캯��
			range_initialize(first, last, iterator_category(first))
		}
		deque(std::initializer_list<value_type> il) {//ʹ���б���г�ʼ������ָ������������
			range_initialize(il.begin(), il.end, input_iterator_tag())
		}

		~deque() {//��������
			destroy(start, finish);
			destroy_map_and_nodes();
		}

	public:
		void push_back(const value_type &val) {//��β���
			if (finish.curr != finish.last - 1) {//�鿴����
				construct(finish.curr, val);
				++finish.curr;
			}
			else {
				push_back_aux(val);//��������ʱ�����������
			}
		}

		void push_front(const value_type &val) {//��ͷ���
			if (start.curr != start.first) {//�鿴����
				construct(start.curr - 1, val);
				--start.curr;
			}
			else {
				push_front_aux(val);//��������ʱ�����������
			}
		}

		void pop_back() {
			if (finish.curr != finish.first) {//��β����
				--finish.curr;
				destroy(finish.curr);
			}
			else {
				pop_back_aux();//����Ҫ�ͷŻ�����
			}
		}

		void pop_front() {//��ͷ����
			if (start.curr + 1 != start.last) {
				destroy(start.curr);
				++start.curr;
			}
			else {
				pop_front_aux();
			}

		}
	public:
		iterator insert(iterator pos, const value_type &val) {//���в���
			if (pos.curr == start.curr) {//�ڶ�ͷֱ�Ӳ���
				push_front(val);
				return start;
			}
			else if (pos.curr == finish.curr) {//�ڶ�βֱ�Ӳ���,���ص�����
				push_back(val);
				iterator tmp = finish;
				--tmp;
				return tmp;
			}
			else {
				return insert_aux(pos, val);//�ڶ��в���
			}
		}

	protected:
		iterator insert_aux(iterator pos, const value_type &val);
	protected:
		void push_back_aux(const value_type&);//��β��ӻ���
		void push_front_aux(const value_type&);//��ͷ��ӻ���
		void pop_back_aux();//��β��������
		void pop_front_aux();//��ͷ��������
	public:
		iterator erase(iterator pos);//�Ƴ�������ָ������
		iterator erase(iterator first, iterator last);//�Ƴ���������Χ�ڵ�Ԫ��
		void clear();//�����������
	protected:
		void create_map_and_nodes(size_type num_elements);
		void destroy_map_and_nodes();
		void fill_initialize(size_type n, const value_type&);
		template<class InputIterator>
		void range_initialize(InputIterator first, InputIterator last, input_iterator_tag);
		template<class ForwardIterator>
		void range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag);



	protected:
		pointer allocate_node() { return data_allocator::allocate(buffer_size()); }//Ϊ�����������ڴ�
		void deallocate_node(pointer ptr) {//Ϊ�������ͷ��ڴ�
			return data_allocator::deallocate(ptr, buffer_size());
		}
		void reallocate_map_front(size_type nodes_to_add = 1) {//�ж�map����û��Ϊ��ͷ�������
			if (nodes_to_add + map > start.node) {
				reallocate_map(nodes_to_add, true);
			}
		}
		void reallocate_map_back(size_type nodes_to_add = 1) {
			if (nodes_to_add + 1 > map_size-(finish.node - map)) {//�ж�map����û��Ϊ��β�������
				reallocate_map(nodes_to_add, false);
			}
		}
		void reallocate_map(size_type nodes_to_add, bool add_front);//����map�ռ�
	};

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::create_map_and_nodes(size_type num_elements) {//�����������ĺͷ��仺�����ڴ�
		size_type num_nodes = num_elements / buffer_size() + 1;//���㴴������������
		map_size = max(initial_map_size(), num_nodes + 2);//��Ĭ��ֵ�Ƚϣ�ȡ���
		map = map_alloctor::allocate(map_size);//������������
		map_pointer nstart = map + (map_size - num_nodes) / 2;//ʹ�������Ŀ�ͷָ�����м�λ��
		map_pointer nfinish = nstart + num_nodes - 1;//��������βָ��
		map_pointer curr;//��������Ŀǰָ��
		try {
			for (curr = nstart; curr <= nfinish; ++curr) {//ѭ�����仺�������ڴ�
				*curr = allocate_node();
			}
		}
		catch (...) {//�쳣�ͷ��ڴ�
			for (map_pointer n = nstart; n < curr; ++n) {
				deallocate_node(*n);
			}
			map_alloctor::deallocate(map, map_size);
			throw;
		}
		start.set_node(nstart);//���ÿ�������ͷָ��
		finish.set_node(nfinish);//���ÿ�������βָ��
		start.curr = start.first;//ͷ��������Ŀǰָ��
		finish.curr = finish.first + num_elements % (buffer_size());//β��������Ŀǰָ��
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::destroy_map_and_nodes() {//���ٿ������ĺ��ͷŻ������ڴ�
		for (map_pointer curr = start.node; curr <= finish.node; ++curr) {
			deallocate_node(*curr);
		}
		map_alloctor::deallocate(map, map_size);
	}

	template<class T, class Alloc, size_t Bufsize>
	typename deque<T, Alloc, Bufsize>::iterator
		deque<T, Alloc, Bufsize>::erase(iterator pos) {//�Ƴ�������ָ���Ԫ��
		iterator next = pos;
		++next;
		difference_type index = pos - start;
		if (index < (size() >> 1)) {//��ǰ��Ԫ�ؽ���,����ǰ��
			copy_backward(start, pos, next);//��start��pos����posԪ��ȫ��copy��next֮ǰ���Ӻ��濪ʼ
			pop_front();//������ͷ����ĵ�һ��Ԫ��
		}
		else {//�������ȽϷ���
			copy(next, finish, pos);//��next��finish copy��pos��ʼ�����pos����
			pop_back();//������β�����Ԫ��
		}
		return start + index;
	}

	template<class T, class Alloc, size_t Bufsize>
	typename deque<T, Alloc, Bufsize>::iterator
		deque<T, Alloc, Bufsize>::erase(iterator first, iterator last) {//�Ƴ���������Χ�ڵ�Ԫ��
		if (first == start&&finifsh == last) {//��Χ�պ��ǿ�������ͷ��β��ȫ�����
			clear();
			return finish;
		}
		else {
			difference_type n = last - first;//����Ԫ��
			difference_type elems_before = first - start;//ǰ���Ԫ����
			if (elems_before < (size() - n) / 2) {//��ǰ���Ԫ�ؽ���
				copy_backward(start, first, last);//��start��first����firstԪ��ȫ��copy��last֮ǰ���Ӻ��濪ʼ
				iterator new_start = start + n;
				destroy(start, new_start);//������Χ�ڵĶ���
				for (map_pointer curr_node = start.node; curr_node < new_start.node; ++cur_node) {//�ͷŷ�Χ�ڴ�
					data_allocator::deallocate(*curr_node, buffer_size());
				}
				start = new_start;//����ͷ������
			}
			else {//��ǰ���Ԫ�ؽ϶�
				copy(last, finish, first);//��last��finish����finishȫ��Ԫ�ش�first��ʼcopy����
				iterator new_finish = finish - n;//
				destroy(new_finish, finish);//��������Χ�ڶ���
				for (map_pointer cur_node = new_finish.node + 1; cur <= finish.node; ++cur_node) {
					data_allocator::deallocate(*cur_node, buffer_size());//�ͷŷ�Χ�ڴ�
				}
				finish = new_finish;//����β������
			}
			return start + elems_before;//��һ��������ص�last��λ�õ��������ڶ���������ص�firstλ�õĵ�����
		}
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::clear() {//�������deque,���ǻᱣ��ͷ����һ��������
		for (map_pointer cur_node = start.node + 1; cur_node < finish.node; ++cur_node) {
			destroy(*cur_node, *cur_node + buffer_size());//����ÿ��������������Ԫ��
			data_allocator::deallocate(*cur_node, buffer_size());//�ͷ�ÿ�����������ڴ�
		}
		if (start.node != finish.node) {//ͷβ����ͬһ��������������ָ������������
			destroy(start.curr, start.last);
			destroy(finish.first, finish.curr);
			data_allocator::deallocate(finish.first, buffer_size());
		}
		else {
			destroy(start.curr, start.last);//ͷ������������������
		}
		finish = start;//ͷβ����
	}





	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::fill_initialize(size_type n, const value_type &val) {//������ʼ��
		create_map_and_nodes(n);
		map_pointer curr;
		try {
			for (curr = start.node; curr < finish.node; ++curr) {//��ʼ���������ڿռ�
				uninitialized_fill(*curr, *curr + buffer_size(), val);
			}
			uninitialized_fill(finish.first, finish.curr, val);
		}
		catch (...) {//�쳣������
			for (map_pointer ptr = start.node; ptr < curr; ++ptr) {
				destroy(*ptr, *ptr + buffer_size());
			}
			destroy_map_and_nodes();
		}
	}


	template<class T, class Alloc, size_t Bufsize>
	template<class ForwardIterator>
	void deque<T, Alloc, Bufsize>::range_initialize(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {//��Χ��ʼ��
		size_type n = 0;
		__distance(first, last, n);//��������������֮��ľ���
		create_map_and_nodes(n);//�����������ĺͷ��仺�����ڴ�
		try {
			uninitialized_copy(first, last, start);//�������Ƴ�ʼ��
		}
		catch (...) {
			destroy_map_and_nodes();
		}
	}

	template<class T, class Alloc, size_t Bufsize>//����
	template<class InputIterator>
	void deque<T, Alloc, Bufsize>::range_initialize(InputIterator first, InputIterator last, input_iterator_tag) {//��Χ��ʼ��
		create_map_and_nodes(0);
		for (; first != last; ++first) {
			push_back(*first);//ѭ�����
		}
	}


	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::reallocate_map(size_type nodes_to_add, bool add_front) {//��������map
		size_type old_num_nodes = finish.node - start.node + 1;//ԭ����������
		size_type new_num_nodes = old_num_nodes + nodes_to_add;//�»���������
		map_pointer new_start;
		if (map_size > 2 * new_num_nodes) {//�ռ乻����
			new_start = map + (map_size - new_num_nodes) / 2 + (add_front ? nodes_to_add : 0);//map�µ�ͷָ��
			if (new_start < start.node) {//�µ�ͷָ����start���
				copy(start.node, finish.node + 1, new_start);//��start��finish���µ�ͷָ�뿪ʼcopy
			}
			else {
				copy_backward(start.node, finish.node + 1, new_start + old_num_nodes);//��start��finish copy��new_start + old_num_nodes,��β��ʼ
			}
		}
		else {//�ռ䲻������
			size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;//������map�Ĵ�С
			map_pointer new_map = map_alloctor::allocate(new_map_size);//�����ڴ�
			new_start = new_map + (new_map_size - new_num_nodes) / 2 + (add_front ? nodes_to_add : 0);//�µ�ͷָ��
			copy(start.node, finish.node + 1, new_start);//��start��finish��new_start��ʼcopy
			map_alloctor::deallocate(map, map_size);//�ͷž�map�ڴ�
			map = new_map;
			map_size = new_map_size;
		}
		start.set_node(new_start);//����start
		finish.set_node(start.node + old_num_nodes - 1);//����finish
	}

	template<class T, class Alloc, size_t Bufsize>
	typename deque<T, Alloc, Bufsize>::iterator
		deque<T, Alloc, Bufsize>::insert_aux(iterator pos, const value_type &val) {
		difference_type index = pos - start;//�����ǰ��Ԫ�ظ���
		value_type val_copy = val;
		if (index < size() / 2) {//ǰ��Ԫ�رȽ���
			push_front(front());//���ڶ�ͷ�ظ������һ��Ԫ��
			iterator front1 = start;
			++front1;//start+1 ԭ��ͷ
			iterator front2 = front1;
			++front2;//ԭ��ͷ+1 �ǵڶ���Ԫ��
			pos = start + index;
			iterator pos1 = pos;
			++pos1;//pos+1 ԭҪ������һ�ε�β
			copy(front1, pos1, front2);//��front1��pos1��front��ʼcopy
		}
		else {//�����������Ԫ����
			push_back(back());//���ڶ�β�ظ��������һ��Ԫ��
			iterator back1 = finish;
			--back1;//ԭ��β
			iterator back2 = back1;
			--back2;//ԭ��β-1
			pos = start + index;
			copy_backward(pos, back2, back1);//��pos��back2��back1��ʼcopy  ��β����ʼ
		}
		*pos = val_copy;//�ڲ����������ֵ
		return pos;
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::push_back_aux(const value_type&val) {//���һ��������ֻʣһ��Ԫ�ؿռ䣬���ǿռ䲻��ʱ����
		value_type val_copy = val;//����ֵ
		reallocate_map_back();//�ж���������map
		*(finish.node + 1) = allocate_node();//��������һ���»��������ڴ�
		try {
			construct(finish.curr, val_copy);//������ֵ�������
			finish.set_node(finish.node + 1);//��β������������node
			finish.curr = finish.first;//������β������
		}
		catch (...) {
			deallocate_node(*(finish.node + 1));
			throw;
		}
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::push_front_aux(const value_type &val) {//���һ��������ֻʣһ��Ԫ�ؿռ䣬���ǿռ䲻��ʱ����
		value_type val_copy = val;//����ֵ
		reallocate_map_front();//�ж�Ҫ��Ҫ�����µ�map
		*(start.node - 1) = allocate_node();//��������һ���»��������ڴ� ��ͷ
		try {
			start.set_node(start.node - 1);//��ͷ������������node
			start.curr = start.last - 1;//curr���ó��»�������β
			construct(start.curr, val);//�������ֵ�����ڴ湹�����
		}
		catch (...) {
			start.set_node(start.node + 1);
			start.curr = start.first;
			deallocate_node(*(start.node - 1));
			throw;
		}
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::pop_back_aux() {//���һ��������ɶ��û
		deallocate_node(finish.first);//�ͷ���󻺳����ڴ�
		finish.set_node(finish.node - 1);//Ϊfinish����Ϊ��һ��node
		finish.curr = finish.last - 1;
		destroy(finish.curr);//����Ԫ��
	}

	template<class T, class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::pop_front_aux() {//��һ��������ֻ��һ��Ԫ��
		destroy(start.curr);//�������Ԫ��
		deallocate_node(start.first);//�ͷŵ�һ�������ڴ�
		start.set_node(start.node + 1);//Ϊstart������node
		start.curr = start.first;
	}

}


#endif