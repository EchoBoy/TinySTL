#ifndef TINYSTL_SRC_ALLOC_H_
#define TINYSTL_SRC_ALLOC_H_

/**
 * 空间配置器下属内存管理，提供内存分配和回收工具：allocate 和 deallocate。与标准STL不同，这里没有考虑多线程的情况。
 */

#include <cstdlib>

namespace TinySTL {
class alloc {
 private:
  static const int ALIGN = 8; // 小型区块的上调边界
  static const int MAXBYTES = 128; // 小型区块的上界
  static const int NFREELISTS = MAXBYTES / ALIGN; // free-lists 个数
  // free-lists 的节点构造
  struct node {
    struct node *next;
  };

 private:
  static node *free_list[NFREELISTS]; // 管理的自由链表
  static char *start_free; // 内存池起始位置
  static char *end_free; // 内存池结束位置
  static size_t heap_size; // 内存池使用总大小

 private:
  // 向上找第一个大于等于 bytes 的 8 的倍数，例如 bytes 为 17 时，应该调整为 24
  static size_t round_up(size_t bytes) {
    return ((bytes + ALIGN - 1) & ~(ALIGN - 1));
  }
  // 根据内存空间大小，决定其所属的链表编号（也就是free_list的下标)
  static int size2index(size_t bytes) {
    return ((bytes + ALIGN - 1) / ALIGN - 1);
  }
  // 根据链表编号，得到其链表中每个单元的大小
  static size_t index2size(int index) {
    return (index + 1) * ALIGN;
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
