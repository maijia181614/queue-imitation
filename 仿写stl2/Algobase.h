#ifndef ALGOBASE_H
#define ALGOBASE_H
#include "Iterator1.h"
#include "TypeTraits.h"
#include <string.h>
#include "Utility.h"
namespace queueSTL {

	template <class T>
	inline const T& max(const T& a, const T& b) {
		return  a < b ? b : a;
	}

	template<class T>
	inline const T& min(const T &a, const T &b) {
		return b < a ? b : a;
	}

	template <class T, class Compare>
	inline const T& min(const T& a, const T& b, Compare comp) {
		return comp(b, a) ? b : a;
	}

	template <class T, class Compare>
	inline const T& max(const T& a, const T& b, Compare comp) {
		return comp(a, b) ? b : a;
	}
	template<class ForwardIterator1,class ForwardIterator2,class T>
	inline void _iter_swap(ForwardIterator1 x, ForwardIterator2 y, T*) {
		T tmp = *x;
		*x = *y;
		*y = tmp;
	}

	template<class ForwardIterator1, class ForwardIterator2>
	inline void iter_swap(ForwardIterator1 x, ForwardIterator2 y) {
		_iter_swap(x, y, value_type(x));
	}

	template<class InputIterator,class OutputIterator>
	inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result,
		input_iterator_tag) {
		for (; first != last; ++first,++result)
			*result = *first;
		return result;
	}

	template<class RandomAccessIterator,class OutputIterator,class Distance>
	inline OutputIterator _copy_d(RandomAccessIterator first, RandomAccessIterator last,
		OutputIterator result,Distance*) {
		for (Distance n = last - first; n > 0; --n, ++first, ++result)
			*result = *first;
		return result;
	}

	template<class RandomAccessIterator,class OutputIterator>
	inline OutputIterator _copy(RandomAccessIterator first, RandomAccessIterator last,
		OutputIterator result, random_access_iterator_tag) {
		return _copy_d(first, last, result, distance_type(first));
	}

	template<class InputIterator,class OutputIterator>
	struct _copy_dispatch {
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
			return _copy(first, last, result,iterator_category(first));
		}
	};

	template<class T>
	inline T* _copy_t(const T *first, const T *last, T *result, _true_type) {
		memmove(result, first, sizeof(T)*(last - first));
		return result + (last - first);
	}

	template<class T>
	inline T* _copy_t(const T *first, const T *last, T *result, _false_type) {
		return _copy_d(first, last, result, (ptrdiff_t*)(0));
	}

	template<class T>
	struct _copy_dispatch<T*, T*> {
		T* operator()(T *first, T *last, T *result) {
			typedef typename _type_traits<T>::has_trivial_assignment_operator t;
			return _copy_t(first, last, result, t());
		}
	};

	template<class T>
	struct _copy_dispatch<const T*, const T*> {
		T* operator()(const T *first,const T *last, T *result) {
			typedef typename _type_traits<T>::has_trivial_assignment_operator t;
			return _copy_t(first, last, result, t());
		}
	};

	template <class InputIterator, class OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last,
		OutputIterator result)
	{
		return _copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}

	inline char* copy(const char* first, const char* last, char* result) {
		memmove(result, first, last - first);
		return result + (last - first);
	}

	inline wchar_t* copy(const wchar_t* first, const wchar_t* last,
		wchar_t* result) {
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}

	template<class BidirectionalIterator1, class BidirectionalIterator2>
	inline BidirectionalIterator2 _copy_backward(BidirectionalIterator1 first,
		BidirectionalIterator1 last, BidirectionalIterator2 result) {
		while (first != last)
			*--result = *--last;
		return result;
	}

	template<class BidirectionalIterator1, class BidirectionalIterator2>
	struct _copy_backward_dispatch {
		BidirectionalIterator2 operator()(BidirectionalIterator1 first,
			BidirectionalIterator1 last, BidirectionalIterator2 result) {
			return _copy_backward(first, last, result);
		}
	};

	template<class T>
	inline T* _copy_backward_t(const T *first, const T *last, T *result, _true_type) {
		const ptrdiff_t N = last - first;
		memmove(result - N, first, sizeof(T)*N);
		return result - N;
	}

	template<class T>
	inline T* _copy_backward_t(const T *first, const T *last, T *result, _false_type) {
		return _copy_backward(first, last, result);
	}

	template<class T>
	struct _copy_backward_dispatch<T*, T*> {
		T* operator()(T *first, T *last, T *result) {
			typedef typename _type_traits<T>::has_trivial_assignment_operator t;
			return _copy_backward_t(first, last, result, t());
		}
	};

	template<class T>
	struct _copy_backward_dispatch<const T*, const T*> {
		T* operator()(const T *first, const T *last, T *result) {
			typedef typename _type_traits<T>::has_trivial_assignment_operator t;
			return _copy_backward_t(first, last, result, t());
		}
	};

	template <class BidirectionalIterator1, class BidirectionalIterator2>
	inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first,
		BidirectionalIterator1 last,BidirectionalIterator2 result) {
		return _copy_backward_dispatch<BidirectionalIterator1,
			BidirectionalIterator2>()(first, last,
				result);
	}

	template <class InputIterator, class Size, class OutputIterator>
	pair<InputIterator, OutputIterator> _copy_n(InputIterator first, Size count,
		OutputIterator result,input_iterator_tag) {
		for (; count > 0; --count, ++first, ++result)
			*result = *first;
		return pair<InputIterator, OutputIterator>(first, result);
	}

	template <class RandomAccessIterator, class Size, class OutputIterator>
	inline pair<RandomAccessIterator, OutputIterator>
		_copy_n(RandomAccessIterator first, Size count,OutputIterator result,random_access_iterator_tag) {
		RandomAccessIterator last = first + count;
		return pair<RandomAccessIterator, OutputIterator>(last,copy(first, last, result));
	}

	template <class InputIterator, class Size, class OutputIterator>
	inline pair<InputIterator, OutputIterator>
		copy_n(InputIterator first, Size count,
			OutputIterator result) {
		return _copy_n(first, count, result, iterator_category(first));
	}

	template <class ForwardIterator, class T>
	void fill(ForwardIterator first, ForwardIterator last, const T& value) {
		for (; first != last; ++first)
			*first = value;
	}

	template <class OutputIterator, class Size, class T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
		for (; n > 0; --n, ++first)
			*first = value;
		return first;
	}

	template <class InputIterator1, class InputIterator2>
	pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1,
		InputIterator1 last1,
		InputIterator2 first2) {
		while (first1 != last1 && *first1 == *first2) {
			++first1;
			++first2;
		}
		return pair<InputIterator1, InputIterator2>(first1, first2);
	}

	template <class InputIterator1, class InputIterator2, class BinaryPredicate>
	pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1,
		InputIterator1 last1,
		InputIterator2 first2,
		BinaryPredicate binary_pred) {
		while (first1 != last1 && binary_pred(*first1, *first2)) {
			++first1;
			++first2;
		}
		return pair<InputIterator1, InputIterator2>(first1, first2);
	}

	template <class InputIterator1, class InputIterator2>
	inline bool equal(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2) {
		for (; first1 != last1; ++first1, ++first2)
			if (*first1 != *first2)
				return false;
		return true;
	}

	template <class InputIterator1, class InputIterator2, class BinaryPredicate>
	inline bool equal(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, BinaryPredicate binary_pred) {
		for (; first1 != last1; ++first1, ++first2)
			if (!binary_pred(*first1, *first2))
				return false;
		return true;
	}

	template <class InputIterator1, class InputIterator2>
	bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2) {
		for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
			if (*first1 < *first2)
				return true;
			if (*first2 < *first1)
				return false;
		}
		return first1 == last1 && first2 != last2;
	}

	template <class InputIterator1, class InputIterator2, class Compare>
	bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2,
		Compare comp) {
		for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
			if (comp(*first1, *first2))
				return true;
			if (comp(*first2, *first1))
				return false;
		}
		return first1 == last1 && first2 != last2;
	}

	inline bool
		lexicographical_compare(const unsigned char* first1,const unsigned char* last1,
			const unsigned char* first2,const unsigned char* last2)
	{
		const size_t len1 = last1 - first1;
		const size_t len2 = last2 - first2;
		const int result = memcmp(first1, first2, min(len1, len2));
		return result != 0 ? result < 0 : len1 < len2;
	}
}

#endif
