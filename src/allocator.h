#ifndef TINYSTL_SRC_ALLOCATOR_H_
#define TINYSTL_SRC_ALLOCATOR_H_

/**
 * 空间配置器，集成 alloc.h
 */

#include "./alloc.h"

namespace TinySTL {

template<typename T>
class allocator {
 public:
  // TODO: static_case 和 普通类型转换有什么区别？
  static T *allocate() { return static_cast<T *>(alloc::allocate(sizeof(T))); }
  static T *allocate(size_t n) {
    if (n == 0)
      return nullptr;
    return static_cast<T *>(alloc::allocate(sizeof(T) * n));
  }
  static void deallocate(T *ptr) { alloc::deallocate(ptr, sizeof(T)); }
  static void deallocate(T *ptr, size_t n) {
    if (n == 0)
      return;
    alloc::deallocate(ptr, sizeof(T) * n);
  }
};
}

#endif //TINYSTL_SRC_ALLOCATOR_H_
