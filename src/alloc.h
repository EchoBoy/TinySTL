#ifndef TINYSTL_SRC_ALLOC_H_
#define TINYSTL_SRC_ALLOC_H_

/**
 * 空间配置器下属内存管理，提供内存分配和回收工具：allocate 和 deallocate。与标准STL不同，这里没有考虑多线程的情况。
 */

#include <cstdlib>

namespace TinySTL {
class alloc {
 private:
  // TODO: 为何使用 ENUM 来定义常量？
  enum EAlign { ALIGN = 8 }; // 小型区块的上调边界
  enum EMaxBytes { MAXBYTES = 128 }; // 小型区块的上界
  enum ENFreeLists { NFREELISTS = (EMaxBytes::MAXBYTES / EAlign::ALIGN) }; // free-lists 个数
  // free-lists 的节点构造 TODO: 定义union的目的？
  union node {
    union node *next;
    char client[1];
  };

 private:
  static node *free_list[ENFreeLists::NFREELISTS]; // 管理的自由链表
  static char *start_free; // 内存池起始位置
  static char *end_free; // 内存池结束位置
  static size_t heap_size; // 内存池使用总大小

 private:
  // 向上找第一个大于等于 bytes 的 8 的倍数，例如 bytes 为 17 时，应该调整为 24
  static size_t round_up(size_t bytes) {
    return ((bytes + EAlign::ALIGN - 1) & ~(EAlign::ALIGN - 1));
  }
  // 根据内存空间大小，决定其所属的链表编号（也就是free_list的下标)
  static int size2index(size_t bytes) {
    return ((bytes + EAlign::ALIGN - 1) / EAlign::ALIGN - 1);
  }
  // 根据链表编号，得到其链表中每个单元的大小
  static size_t index2size(int index) {
    return (index + 1) * EAlign::ALIGN;
  }
  // 向下标为 index 的自由链表中添加新节点，返回填充的个数。
  static size_t refill(int index);
  // 配置一大块空间，可容纳 nobjs 个大小为 size 的区块，如果配置 nobjs 个区块有所不便，nobjs 可能会降低
  static char *chunk_alloc(size_t size, int *nobjs);

 public:
  static void *allocate(size_t bytes);
  static void deallocate(void *ptr, size_t bytes);
  static void *reallocate(void *ptr, size_t old_size, size_t new_size);
};
}

#endif //TINYSTL_SRC_ALLOC_H_
