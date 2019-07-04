#ifndef TINYSTL_SRC_ALLOCATOR_H_
#define TINYSTL_SRC_ALLOCATOR_H_

/**
 * 空间配置器，集成 __alloc.h, __construct.h
 */

#include "__alloc.h"
#include "__construct.h"

namespace TinySTL {

template<typename T>
class allocator {
 public:
  // ok_TODO: static_case 和 普通类型转换有什么区别？ ans：char * to int *，普通转换能编译，static_case转换不能编译。
  static T *allocate() { return static_cast<T *>(__alloc::allocate(sizeof(T))); }
  static T *allocate(size_t n) {
    if (n == 0)
      return nullptr;
    return static_cast<T *>(__alloc::allocate(sizeof(T) * n));
  }
  static void deallocate(T *ptr) { __alloc::deallocate(ptr, sizeof(T)); }
  static void deallocate(T *ptr, size_t n) {
    if (n == 0)
      return;
    __alloc::deallocate(ptr, sizeof(T) * n);
  }

  template<typename T1, typename T2>
  static void construct(T1 *ptr, const T2 &val) {
    return __construct::construct(ptr, val);
  }
  template<typename T1>
  static void destroy(T1 *ptr) {
    return __construct::destroy(ptr);
  }
  template<typename ForwardIterator>
  static void destroy(ForwardIterator first, ForwardIterator last) {
    return __construct::destroy(first, last);
  }
};
}

#endif //TINYSTL_SRC_ALLOCATOR_H_
