#ifndef TINYSTL_SRC_UNINITIALIZED_H_
#define TINYSTL_SRC_UNINITIALIZED_H_

#include "./type_traits.h"
#include "./algorithm.h"
#include "./construct.h"

namespace TinySTL {
/** ---------------------- uninitialized_copy ---------------------------- */
template<typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator
__uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T *) {
  typedef typename __type_traits<T>::is_POD_type is_POD_type;
  return __uninitialized_copy_aux(first, last, result, is_POD_type());
}

template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
  return copy(first, last, result);
}
template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator
__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
  ForwardIterator cur = result;
  for (; first != last; ++first, ++cur) {
    construct(&*cur, *first);
  }
  return cur;
}

template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
  return __uninitialized_copy(first, last, result, value_type(result));
}

/** ---------------------- uninitialized_fill ---------------------------- */
template<typename ForwardIterator, typename T>
void
uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x) {
  __uninitialized_fill(first, last, x, value_type(first));
}

template<typename ForwardIterator, typename T, typename T1>
void
__uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &x, T1 *) {
  typedef typename __type_traits<T1>::is_POD_type is_POD_type;
  __uninitialized_fill_aux(first, last, x, is_POD_type());
}
template<typename ForwardIterator, typename T>
void
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __true_type) {
  fill(first, last, x);
}
template<typename ForwardIterator, typename T>
void
__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T &x, __false_type) {
  for (; first != last; ++first) {
    construct(&*first, x);
  }
}

/** ---------------------- uninitialized_fill_n ---------------------------- */
template<typename ForwardIterator, typename Size, typename T, typename T1>
inline ForwardIterator
__uninitialized_fill_n(ForwardIterator first, Size n, const T &x, T1 *) {
  typedef typename __type_traits<T1>::is_POD_type is_POD_type;
  return __uninitialized_fill_n_aux(first, n, x, is_POD_type());
}

template<typename ForwardIterator, typename Size, typename T>
inline ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __true_type) {
  return fill_n(first, n, x);
}

template<typename ForwardIterator, typename Size, typename T>
inline ForwardIterator
__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T &x, __false_type) {
  for (; n > 0; --n, ++first) {
    construct(&*first, x);
  }
  return first;
}

template<typename ForwardIterator, typename Size, typename T>
inline ForwardIterator
uninitialized_fill_n(ForwardIterator first, Size n, const T &x) {
  return __uninitialized_fill_n(first, n, x, value_type(first));
}
}

#endif //TINYSTL_SRC_UNINITIALIZED_H_
