#ifndef TINYSTL_SRC_CONSTRUCT_H_
#define TINYSTL_SRC_CONSTRUCT_H_

/**
 * 空间配置器下属，提供构造和析构基本工具：construct 和 destroy。
 */

#include <new>

#include "./type_traits.h"
#include "./iterator.h"

namespace TinySTL {

template<typename T1, typename T2>
inline void construct(T1 *ptr, const T2 &value) {
  new(ptr) T1(value);
}

template<typename T>
inline void destroy(T *ptr) {
//ok_TODO: 基础类型（如int），有析构函数？有没有都无所谓，有的话会更统一。
  ptr->~T();
}

template<typename ForwardIterator, typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T *) {
  typedef typename __type_traits<T>::has_trivial_destructor has_trivial_destructor;
  __destroy_aux(first, last, has_trivial_destructor());
}

template<typename ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {}

template<typename ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
  for (; first < last; ++first) {
    destroy(&*first);
  }
}

template<typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
  __destroy(first, last, value_type(first));
}

}

#endif //TINYSTL_SRC_CONSTRUCT_H_
