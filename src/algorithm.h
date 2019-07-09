#ifndef TINYSTL_SRC_ALGORITHM_H_
#define TINYSTL_SRC_ALGORITHM_H_

#include "iterator.h"
#include "type_traits.h"

namespace TinySTL {

/***************** [swap] T(n) = O(1) *********************/
template<typename T>
inline void
swap(T &a, T &b) {
  T tmp = a;
  a = b;
  b = tmp;
}

/***************** [max] T(n) = O(1) *********************/
template<typename T>
inline const T &
max(const T &a, const T &b) {
  return a < b ? b : a;
}
template<typename T, typename Compare>
inline const T &
max(const T &a, const T &b, Compare comp) {
  return comp(a, b) ? b : a;
}

/***************** [fill] T(n) = O(n) *********************/
template<typename ForwardIterator, typename T>
inline void
fill(ForwardIterator first, ForwardIterator last, const T &val) {
  for (; first != last; ++first)
    *first = val;
}
inline void
fill(char *first, char *last, const char &val) {
  memset(first, static_cast<unsigned char>(val), last - first);
}

/***************** [fill-n] T(n) = O(n) *********************/
template<typename OutputIterator, typename Size, typename T>
inline OutputIterator
fill_n(OutputIterator first, Size n, const T &val) {
  for (; n > 0; --n, ++first)
    *first = val;
  return first;
}
template<typename Size>
inline char *
fill_n(char *first, Size n, const char &val) {
  memset(first, static_cast<unsigned char>(val), n);
  return first + n;
}

/***************** [copy-backward] T(n) = O(n) *********************/
// 出口一：assignment operator: 其他
template<typename InputIterator, typename BidirectionalIterator>
inline BidirectionalIterator
__copy_backward(InputIterator first, InputIterator last, BidirectionalIterator result) {
  --last;
  --result;
  for (auto n = TinySTL::distance(first, last); n > 0; --n, --result, --last)
    *result = *last;
  return result;
}
// 出口二：memmove，条件：原生指针 + has_trivial_assignment_operator
template<typename T>
inline T *
__copy_t_backward(const T *first, const T *last, T *result, __true_type) {
  auto dist = last - first;
  memmove(result - dist, first, sizeof(T) * dist);
  return result - dist;
}
template<typename T>
inline T *
__copy_t_backward(const T *first, const T *last, T *result, __false_type) {
  return TinySTL::__copy_backward(first, last, result);
}

template<typename InputIterator, typename OutputIterator>
struct __copy_dispatch_backward {
  OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
    return __copy_backward(first, last, result);
  }
};
template<typename T>
struct __copy_dispatch_backward<T *, T *> {
  T *operator()(T *first, T *last, T *result) {
    typedef typename __type_traits<T>::has_trivial_assignment_operator t;
    return __copy_t_backward(first, last, result, t());
  }
};
template<typename T>
struct __copy_dispatch_backward<const T *, T *> {
  T *operator()(const T *first, const T *last, T *result) {
    typedef typename __type_traits<T>::has_trivial_assignment_operator t;
    return __copy_t_backward(first, last, result, t());
  }
};

template<typename InputIterator, typename OutputIterator>
inline OutputIterator
copy_backward(InputIterator first, InputIterator last, OutputIterator result) {
  return __copy_dispatch_backward<InputIterator, OutputIterator>()(first, last, result);
}
inline char *
copy_backward(const char *first, const char *last, char *result) {
  auto dist = last - first;
  memmove(result - dist, first, dist);
  return result - dist;
}

/***************** [copy] T(n) = O(n) *********************/
// 出口一：assignment operator: 其他
template<typename InputIterator, typename OutputIterator>
inline OutputIterator
__copy(InputIterator first, InputIterator last, OutputIterator result) {
  for (auto n = TinySTL::distance(first, last); n > 0; --n, ++result, ++first)
    *result = *first;
  return result;
}
// 出口二：memmove，条件：原生指针 + has_trivial_assignment_operator
template<typename T>
inline T *
__copy_t(const T *first, const T *last, T *result, __true_type) {
  memmove(result, first, sizeof(T) * (last - first));
  return result + (last - first);
}
template<typename T>
inline T *
__copy_t(const T *first, const T *last, T *result, __false_type) {
  return __copy(first, last, result);
}

template<typename InputIterator, typename OutputIterator>
struct __copy_dispatch {
  OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
    return TinySTL::__copy(first, last, result);
  }
};
template<typename T>
struct __copy_dispatch<T *, T *> {
  T *operator()(T *first, T *last, T *result) {
    typedef typename __type_traits<T>::has_trivial_assignment_operator t;
    return __copy_t(first, last, result, t());
  }
};
template<typename T>
struct __copy_dispatch<const T *, T *> {
  T *operator()(const T *first, const T *last, T *result) {
    typedef typename __type_traits<T>::has_trivial_assignment_operator t;
    return __copy_t(first, last, result, t());
  }
};

template<typename InputIterator, typename OutputIterator>
inline OutputIterator
copy(InputIterator first, InputIterator last, OutputIterator result) {
  return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
}
inline char *
copy(const char *first, const char *last, char *result) {
  memmove(result, first, last - first);
  return result + (last - first);
}
inline wchar_t *
copy(const wchar_t *first, const wchar_t *last, wchar_t *result) {
  memmove(result, first, sizeof(wchar_t) * (last - first));
  return result + (last - first);
}

}

#endif //TINYSTL_SRC_ALGORITHM_H_
