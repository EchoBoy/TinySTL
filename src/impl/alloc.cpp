#include <new>
#include "../alloc.h"

namespace TinySTL {

char *alloc::start_free = nullptr;
char *alloc::end_free = nullptr;
size_t alloc::heap_size = 0;
alloc::node *alloc::free_list[alloc::NFREELISTS] = { nullptr };

void *alloc::allocate(size_t bytes) {
  if (bytes > MAXBYTES) {
    return malloc(bytes);
  }
  int index = size2index(bytes);
  node *list = free_list[index];
  if (nullptr == list) {
    refill(index);
    list = free_list[index];
  }
  free_list[index] = list->next;
  return list;
}

void alloc::deallocate(void *ptr, size_t bytes) {
  if (bytes > MAXBYTES) {
    free(ptr);
    return;
  }
  int index = size2index(bytes);
  node *the_node = (node *) ptr;
  the_node->next = free_list[index];
  free_list[index] = the_node;
}

void *alloc::reallocate(void *ptr, size_t old_size, size_t new_size) {
  deallocate(ptr, old_size);
  return allocate(new_size);
}

size_t alloc::refill(int index) {
  // 每次默认添加20个节点
  int nobjs = 20;
  int unit = index2size(index);

  char *chunk = chunk_alloc(unit, &nobjs);
  // 链接链表
  node *old_head = free_list[index];
  node *new_head = (node *) (chunk);
  node *p = new_head;
  for (int i = 1; i < nobjs; i++) {
    p->next = (node *) (chunk + (i * unit));
    p = p->next;
  }
  p->next = old_head;
  free_list[index] = new_head;
  return nobjs;
}

char *alloc::chunk_alloc(size_t size, int *nobjs) {
  char *res;
  size_t total_bytes = size * (*nobjs);
  size_t byte_left = end_free - start_free;

  if (byte_left >= total_bytes) {
    // 内存池剩余空间完成够用，直接分配
    res = start_free;
    start_free += total_bytes;
    return res;
  }

  if (byte_left >= size) {
    // 内存池剩余空间不能完全满足，但能够供应一个及以上区块
    *nobjs = byte_left / size;
    total_bytes = *nobjs * size;
    res = start_free;
    start_free += total_bytes;
    return res;
  }

  // 内存池剩余空间连一个区块都不能供应
  size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);
  // 回收剩下的内存
  if (byte_left > 0) {
    // ok_TODO: 应该将 byte_left 放入 free_list[index-1]中？ans：这里 byte_left 肯定是 8 的倍数
    int index = size2index(byte_left);
    ((node *) start_free)->next = free_list[index];
    free_list[index] = (node *) start_free;
  }
  start_free = (char *) malloc(bytes_to_get);
  if (nullptr == start_free) {
    // malloc 空间分配失败
    for (int i = size2index(size) + 1; i < NFREELISTS; i++) {
      if (nullptr != free_list[i]) {
        start_free = (char *) free_list[i];
        end_free = start_free + index2size(i);
        free_list[i] = free_list[i]->next;
        // 递归调用自己，为了修正 nobjs
        return chunk_alloc(size, nobjs);
      }
    }
    // ok_TODO: 如何正确的抛出异常。ans：throw 就像函数返回值一样，没有规定类型，但一般使用有意义的exception（std::exception的子类）
    throw std::bad_alloc();
  }
  heap_size += bytes_to_get;
  end_free = start_free + bytes_to_get;
  // 递归调用自己，为了修正 nobjs
  return chunk_alloc(size, nobjs);
}
}
