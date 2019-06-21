#ifndef TINYSTL_SRC_ALGORITHM_H_
#define TINYSTL_SRC_ALGORITHM_H_

#include "./iterator.h"
#include "./type_traits.h"

namespace TinySTL {
/***************** [max] T(n) = O(n) *********************/
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
//  TODO: = 与拷贝构造函数用法区别
  for (; first != last; ++first)
    *first = val;
}
inline void
fill(char *first, char *last, const char &val) {
  memset(first, static_cast<unsigned char>(val), last - first);
}
inline void
fill(wchar_t *first, wchar_t *last, const wchar_t &val) {
  memset(first, static_cast<unsigned char>(val), (last - first) * sizeof(wchar_t));
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
fill_n(char *first, char *last, Size n, const char &val) {
  memset(first, static_cast<unsigned char>(val), n);
  return first + n;
}

template<typename Size>
inline wchar_t *
fill_n(wchar_t *first, Size n, const wchar_t &val) {
  memset(first, static_cast<unsigned char>(val), n * sizeof(wchar_t));
  return first + n;
}

/***************** [copy-backward] T(n) = O(n) *********************/

template<typename BidirectionalIterator1, typename BidirectionalIterator2>
inline BidirectionalIterator2
__copy_backward_aux(BidirectionalIterator1 first,
                    BidirectionalIterator1 last,
                    BidirectionalIterator2 result,
                    __true_type) {
  auto dist = distance(first, last);
  memmove(result, first, sizeof(*first) * dist);
  advance(result, -dist);
  return result;
}

template<typename BidirectionalIterator1, typename BidirectionalIterator2>
inline BidirectionalIterator2
__copy_backward_aux(BidirectionalIterator1 first,
                    BidirectionalIterator1 last,
                    BidirectionalIterator2 result,
                    __false_type) {
  --last;
  --result;
  for (; last != first; --last, --result)
    *result = *last;
  *result = *last;
  return result;
}

template<typename BidirectionalIterator1, typename BidirectionalIterator2, typename T>
inline BidirectionalIterator2
__copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result, T *) {
  typedef typename __type_traits<T>::is_POD_type is_pod;
  return __copy_backward_aux(first, last, result, is_pod());
}
template<typename BidirectionalIterator1, typename BidirectionalIterator2>
inline BidirectionalIterator2
copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
  return __copy_backward(first, last, result, value_type(first));
}

/***************** [copy] T(n) = O(n) *********************/
template<typename RandomIterator, typename OutputIterator, typename Distance>
inline OutputIterator
__copy_d(RandomIterator first, RandomIterator last, OutputIterator result, Distance *) {
  for (Distance n = last - first; n > 0; --n, ++result, ++first)
    *result = *first;
  return result;
}

template<typename InputIterator, typename OutputIterator>
inline OutputIterator
__copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
  for (; first != last; ++result, ++first)
    *result = *first;
  return result;
}
template<typename RandomIterator, typename OutputIterator>
inline OutputIterator
__copy(RandomIterator first, RandomIterator last, OutputIterator result, random_iterator_tag) {
  return __copy_d(first, last, result, distance_type(first));
}

template<typename T>
inline T *
__copy_t(const T *first, const T *last, T *result, __true_type) {
  memmove(result, first, sizeof(T) * (last - first));
  return result + (last - first);
}
template<typename T>
inline T *
__copy_t(const T *first, const T *last, T *result, __false_type) {
  return __copy_d(first, last, result, (ptrdiff_t *) nullptr);
}

template<typename InputIterator, typename OutputIterator>
struct __copy_dispatch {
  OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
    return __copy(first, last, result, iterator_category(first));
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
