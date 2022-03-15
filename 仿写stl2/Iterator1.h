#ifndef ITERATOR1_H
#define ITERATOR1_H
#include <stddef.h>

namespace queueSTL{
	//以下是五种支持不同操作的迭代器种类标签
	struct input_iterator_tag {};//只读
	struct output_iterator_tag {};//只写
	struct forward_iterator_tag : public input_iterator_tag {};//允许写入等
	struct bidirectional_iterator_tag : public forward_iterator_tag {};//可双向移动
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};//可随机读取
	//以下为五种支持不同操作的迭代器声明
	template <class T, class Distance> struct input_iterator {
		typedef input_iterator_tag iterator_category;
		typedef T                  value_type;
		typedef Distance           difference_type;
		typedef T*                 pointer;
		typedef T&                 reference;
	};

	struct output_iterator {
		typedef output_iterator_tag iterator_category;
		typedef void                value_type;
		typedef void                difference_type;
		typedef void                pointer;
		typedef void                reference;
	};

	template <class T, class Distance> struct forward_iterator {
		typedef forward_iterator_tag iterator_category;
		typedef T                    value_type;
		typedef Distance             difference_type;
		typedef T*                   pointer;
		typedef T&                   reference;
	};


	template <class T, class Distance> struct bidirectional_iterator {
		typedef bidirectional_iterator_tag iterator_category;
		typedef T                          value_type;
		typedef Distance                   difference_type;
		typedef T*                         pointer;
		typedef T&                         reference;
	};

	template <class T, class Distance> struct random_access_iterator {
		typedef random_access_iterator_tag iterator_category;
		typedef T                          value_type;
		typedef Distance                   difference_type;
		typedef T*                         pointer;
		typedef T&                         reference;
	};

	template <class Category, class T, class Distance = ptrdiff_t,
	class Pointer = T*, class Reference = T&>
	struct iterator {
		typedef Category  iterator_category;
		typedef T         value_type;
		typedef Distance  difference_type;
		typedef Pointer   pointer;
		typedef Reference reference;
	};

	//以下为不同的迭代器特性萃取机，可获取迭代器准确的category、value_type、difference_type等类型
	template <class Iterator>
	struct iterator_traits {
		typedef typename Iterator::iterator_category iterator_category;
		typedef typename Iterator::value_type        value_type;
		typedef typename Iterator::difference_type   difference_type;
		typedef typename Iterator::pointer           pointer;
		typedef typename Iterator::reference         reference;
	};

	template <class T>
	struct iterator_traits<T*> {
		typedef random_access_iterator_tag iterator_category;
		typedef T                          value_type;
		typedef ptrdiff_t                  difference_type;
		typedef T*                         pointer;
		typedef T&                         reference;
	};

	template <class T>
	struct iterator_traits<const T*> {
		typedef random_access_iterator_tag iterator_category;
		typedef T                          value_type;
		typedef ptrdiff_t                  difference_type;
		typedef const T*                   pointer;
		typedef const T&                   reference;
	};

	template <class Iterator>
	inline typename iterator_traits<Iterator>::iterator_category
		iterator_category(const Iterator&) {
			typedef typename iterator_traits<Iterator>::iterator_category category;
			return category();
		}

	template <class Iterator>
	inline typename iterator_traits<Iterator>::difference_type*
		distance_type(const Iterator&) {
			return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
		}

	template <class Iterator>
	inline typename iterator_traits<Iterator>::value_type*
		value_type(const Iterator&) {
			return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
		}

	//返回各类迭代器类型
	template <class T, class Distance>
	inline input_iterator_tag
		iterator_category(const input_iterator<T, Distance>&) {
			return input_iterator_tag();
		}

	inline output_iterator_tag iterator_category(const output_iterator&) {
		return output_iterator_tag();
	}

	template <class T, class Distance>
	inline forward_iterator_tag
		iterator_category(const forward_iterator<T, Distance>&) {
			return forward_iterator_tag();
		}

	template <class T, class Distance>
	inline bidirectional_iterator_tag
		iterator_category(const bidirectional_iterator<T, Distance>&) {
			return bidirectional_iterator_tag();
		}

	template <class T, class Distance>
	inline random_access_iterator_tag
		iterator_category(const random_access_iterator<T, Distance>&) {
			return random_access_iterator_tag();
		}

	template <class T>
	inline random_access_iterator_tag iterator_category(const T*) {
		return random_access_iterator_tag();
	}
	//返回各类迭代器数据类型
	template <class T, class Distance>
	inline T* value_type(const input_iterator<T, Distance>&) {
		return (T*)(0);
	}

	template <class T, class Distance>
	inline T* value_type(const forward_iterator<T, Distance>&) {
		return (T*)(0);
	}

	template <class T, class Distance>
	inline T* value_type(const bidirectional_iterator<T, Distance>&) {
		return (T*)(0);
	}

	template <class T, class Distance>
	inline T* value_type(const random_access_iterator<T, Distance>&) {
		return (T*)(0);
	}

	template <class T>
	inline T* value_type(const T*) { return (T*)(0); }
	//返回迭代器距离类型
	template <class T, class Distance>
	inline Distance* distance_type(const input_iterator<T, Distance>&) {
		return (Distance*)(0);
	}

	template <class T, class Distance>
	inline Distance* distance_type(const forward_iterator<T, Distance>&) {
		return (Distance*)(0);
	}

	template <class T, class Distance>
	inline Distance*
		distance_type(const bidirectional_iterator<T, Distance>&) {
			return (Distance*)(0);
		}

	template <class T, class Distance>
	inline Distance*
		distance_type(const random_access_iterator<T, Distance>&) {
			return (Distance*)(0);
		}

	template <class T>
	inline ptrdiff_t* distance_type(const T*) { return (ptrdiff_t*)(0); }


	template <class InputIterator, class Distance>
	inline void __distance(InputIterator first, InputIterator last, Distance& n,
		input_iterator_tag) {
		while (first != last) { ++first; ++n; }
	}

	template <class RandomAccessIterator, class Distance>
	inline void __distance(RandomAccessIterator first, RandomAccessIterator last,
		Distance& n, random_access_iterator_tag) {
		n += last - first;
	}

	template <class InputIterator, class Distance>
	inline void distance(InputIterator first, InputIterator last, Distance& n) {
		__distance(first, last, n, iterator_category(first));
	}


	
	//前进
	template <class InputIterator, class Distance>
	inline void __advance(InputIterator& i, Distance n, input_iterator_tag) {
		while (n--) ++i;
	}

	template <class BidirectionalIterator, class Distance>
	inline void __advance(BidirectionalIterator& i, Distance n,
		bidirectional_iterator_tag) {
		if (n >= 0)
			while (n--) ++i;
		else
			while (n++) --i;
	}

	template <class RandomAccessIterator, class Distance>
	inline void __advance(RandomAccessIterator& i, Distance n,
		random_access_iterator_tag) {
		i += n;
	}

	template <class InputIterator, class Distance>
	inline void advance(InputIterator& i, Distance n) {
		__advance(i, n, iterator_category(i));
	}


}

#endif
