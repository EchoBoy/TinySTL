#ifndef TINYSTL_SRC_DEQUE_H_
#define TINYSTL_SRC_DEQUE_H_

#include "allocator.h"
#include "iterator.h"
#include "uninitialized.h"
#include "algorithm.h"

namespace TinySTL {

// TODO: 寻找更优的解决方法
namespace DequeAux {

const int __bucket_bytes = 1024;
/// 一个 buf 可以存几个元素
inline size_t __bucket_cap(size_t element_size) {
  // 默认一个 node 字节数
  // ok_TODO: 这里没有考虑element_size 为零的情况. C++ 规定 class 或 struct 的sizeof最小结果为1。
  return __bucket_bytes > element_size ? __bucket_bytes / element_size : 1;
}
const int __min_map_size = 8;
}

template<typename T, typename Ref, typename Ptr>
class __deque_iterator {
 public:
  // TODO: 待优化，这个函数会频繁调用，影响运行效率
  static size_t bucket_cap() { return DequeAux::__bucket_cap(sizeof(T)); }

  using iterator_category = random_iterator_tag;
  using value_type = T;
  using reference = Ref;
  using pointer = Ptr;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using map_pointer = T **;
  using self_type = __deque_iterator;

  // 成员变量 声明为private
 private:
  pointer cur;
  pointer first;
  pointer last;
  map_pointer node;

 private:
  void set_node(map_pointer new_node) {
    node = new_node;
    first = *new_node;
    last = first + bucket_cap();
  }
 public:
  __deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
  __deque_iterator(pointer _first, pointer _last, pointer _cur, map_pointer _node) :
      cur(_cur), first(_first), last(_last), node(_node) {}
  reference operator*() const { return *cur; }
  pointer operator->() const { return &(operator*()); }
  difference_type operator-(const self_type &x) const {
    if (node == nullptr || x.node == nullptr)
      return 0;
    return bucket_cap() * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
  }
  self_type &operator++() {
    ++cur;
    if (cur == last) {
      set_node(node + 1);
      cur = first;
    }
    return *this;
  }
  self_type operator++(int) {
    auto res = cur;
    ++(*this);
    return res;
  }
  self_type &operator--() {
    if (cur == first) {
      set_node(node - 1);
      cur = last;
    }
    --cur;
    return *this;
  }
  self_type operator--(int) {
    auto res = cur;
    --(*this);
    return res;
  }
  self_type &operator+=(difference_type n) {
    if (n == 0) return *this;
    difference_type offset = n + (cur - first);
    if (offset >= 0 && offset < bucket_cap())
      cur += n;
    else {
      difference_type node_offset =
          offset > 0 ?
          offset / bucket_cap() :
          -difference_type((-offset - 1) / bucket_cap()) - 1;
      set_node(node + node_offset);
      cur = first + (offset - node_offset * bucket_cap());
    }
    return *this;
  }

  self_type operator+(difference_type n) const {
    auto tmp = *this;
    return tmp += n;
  }
  self_type &operator-=(difference_type n) { return *this += -n; }
  self_type operator-(difference_type n) const {
    auto tmp = *this;
    return tmp -= n;
  }

 public:
  friend bool operator==(const self_type &x, const self_type &y) { return y.cur == x.cur; }
  friend bool operator!=(const self_type &x, const self_type &y) { return !(y == x); }
  friend bool operator<(const self_type &x, const self_type &y) {
    return (x.node == y.node) ? (x.cur < y.cur) : (x.node < y.node);
  }

  template<typename>
  friend
  class deque;
};

template<typename T>
class deque {
 public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using iterator = __deque_iterator<T, T &, T *>;
  using const_iterator = __deque_iterator<T, const T &, const T *>;
  using size_type = size_t;

 protected:
  using map_pointer = pointer *;
  using node_allocator = allocator<value_type>;
  using map_allocator = allocator<pointer>;

 protected:
  iterator start;
  iterator finish;
  map_pointer map;
  size_type map_size;
  // 单个 bucket 能包含多少个元素
  size_t bucket_cap;

 public:
  /**** 生命周期：ctor、copy ctor、copy assignment、move ctor、move assignment、dtor ****/
  deque() { init_allocate(0); }
  explicit deque(int n) : deque(n, value_type()) {}
  deque(int n, const value_type &val) {
    init_allocate(n);
    for (auto cur = start.node; cur < finish.node; ++cur)
      TinySTL::uninitialized_fill(*cur, *cur + bucket_cap, val);
    TinySTL::uninitialized_fill(finish.first, finish.cur, val);
  }
  template<typename InputIterator>
  deque(InputIterator first, InputIterator last) {
    init_allocate(TinySTL::distance(first, last));
    TinySTL::uninitialized_copy(first, last, start);
  }
  // Rule of five
  deque(const deque &x) : deque(x.begin(), x.end()) {}
  deque(deque &&x) : deque() { swap(*this, x); }
  ~deque() {
    // 1. 析构
    node_allocator::destroy(start, finish);
    // 2. 释放空间
    for (auto cur = start.node; cur != finish.node; ++cur)
      delete_node(*cur);
    delete_node(*(finish.node));
    map_allocator::deallocate(map);
  }
  // oK_TODO：可能需要改, 采用 copy-and-swap idiom
  deque &operator=(deque x) {
    swap(*this, x);
    return *this;
  }

  /*************** public const 成员函数 ************/
  size_type size() const { return end() - begin(); }
  bool empty() const { return begin() == end(); }

  const_iterator begin() const { return to_const_iterator(start); }
  const_iterator end() const { return to_const_iterator(finish); }
  const_reference operator[](size_type n) const { return *(begin() + n); }
  const_reference front() const { return *begin(); }
  const_reference back() const { return *(end() - 1); }

  /*************** public 成员函数 ************/
  iterator begin() { return start; }
  iterator end() { return finish; }
  reference operator[](size_type n) { return *(begin() + n); }
  reference front() { return *begin(); }
  reference back() { return *(end() - 1); }

  /*************** 插入删除操作相关 ************/
  void push_back(const value_type &val) {
    if (finish.cur != finish.last - 1) {
      // 备用空间够用
      node_allocator::construct(finish.cur, val);
      ++(finish.cur);
    } else
      push_back_aux(val);
  }
  void push_front(const value_type &val) {
    if (start.cur != start.first) {
      --(start.cur);
      node_allocator::construct(start.cur, val);
    } else {
      push_front_aux(val);
    }
  }
  void pop_back() {
    if (finish.cur != finish.first) {
      --finish.cur;
      node_allocator::destroy(finish.cur);
    } else {
      delete_node(finish.first);
      finish.set_node(finish.node - 1);
      finish.cur = finish.last - 1;
      node_allocator::destroy(finish.cur);
    }
  }
  void pop_front() {
    node_allocator::destroy(start.cur);
    ++(start.cur);
    if (start.cur == start.last) {
      delete_node(start.first);
      start.set_node(start.node + 1);
      start.cur = start.first;
    }
  }
  // clear 的策略是保留一个缓冲区
  void clear() {
    for (auto cur = start.node + 1; cur < finish.node; ++cur) {
      node_allocator::destroy(*cur, (*cur) + bucket_cap);
      delete_node(*cur);
    }
    if (start.node != finish.node) {
      node_allocator::destroy(start.cur, start.last);
      node_allocator::destroy(finish.first, finish.cur);
      delete_node(finish.first);
    } else {
      // 这里是只有一个缓冲区的情况
      node_allocator::destroy(start.cur, finish.cur);
    }
    // 这里讲 start 赋值给 finish之后，start.cur 和 finish.cur 也就一样了
    finish = start;
  }

  // 偷懒不写了
//  void insert();
//  void erase();
 public:
  /*************** 我的朋友 ************/
  friend void swap(deque &x, deque &y) {
    // ADL
    using TinySTL::swap;
    swap(x.start, y.start);
    swap(x.finish, y.finish);
    swap(x.map, y.map);
    swap(x.map_size, y.map_size);
    swap(x.bucket_cap, y.bucket_cap);
  }
  friend bool operator==(const deque &x, const deque &y) {
    auto b1 = x.begin(), b2 = y.begin();
    auto e1 = x.end(), e2 = y.end();
    for (; b1 != e1 && b2 != e2; ++b1, ++b2) {
      if (*b1 != *b2)
        return false;
    }
    return b1 == e1 && b2 == e2;
  }
  friend bool operator!=(const deque &x, const deque &y) { return !(x == y); }

  /*************** 辅助函数 ************/
 protected:
  // 仅申请空间，不做构造
  pointer new_node() { return node_allocator::allocate(bucket_cap); }
  // 仅归还空间，不做析构
  void delete_node(pointer p) { node_allocator::deallocate(p, bucket_cap); }
  // 本函数仅做初始化用途
  void init_allocate(size_type num_elements) {
    this->bucket_cap = DequeAux::__bucket_cap(sizeof(value_type));
    // 实际需要的节点个数
    size_type num_nodes = num_elements / bucket_cap + 1;
    // 实际需要 + 预留
    this->map_size = max(DequeAux::__min_map_size, int(num_nodes + 2));

    // 初始化 map
    map = map_allocator::allocate(map_size);
    map_pointer new_start = map + (map_size - num_nodes) / 2;
    map_pointer new_finish = new_start + num_nodes - 1;

    // 初始化 node
    for (auto cur = new_start; cur <= new_finish; ++cur)
      *cur = new_node();

    start.set_node(new_start);
    finish.set_node(new_finish);
    start.cur = start.first;
    finish.cur = finish.first + num_elements % bucket_cap;
  }
  void push_back_aux(const value_type &val) {
    reserve_map_at_back();
    *(finish.node + 1) = new_node();
    node_allocator::construct(finish.cur, val);
    finish.set_node(finish.node + 1);
    finish.cur = finish.first;
  }
  void push_front_aux(const value_type &val) {
    reserve_map_at_front();
    *(start.node - 1) = new_node();
    start.set_node(start.node - 1);
    start.cur = start.last - 1;
    node_allocator::construct(start.cur, val);
  }
  void reserve_map_at_back(size_type nodes_to_add = 1) {
    // 注意右边节点余量的计算方式, 如果其他地方还会用到，考虑抽象成函数
    if (nodes_to_add > map_size - (finish.node - map) - 1)
      reallocate_map(nodes_to_add, false);
  }
  void reserve_map_at_front(size_type nodes_to_add = 1) {
    if (nodes_to_add > start.node - map)
      reallocate_map(nodes_to_add, true);
  }
  void reallocate_map(size_type nodes_to_add, bool add_at_front) {
    size_type old_num_nodes = finish.node - start.node + 1;
    size_type new_num_nodes = old_num_nodes + nodes_to_add;

    map_pointer new_start = nullptr;
    if (map_size > 2 * new_num_nodes) {
      // 如果map一直向一个方向生长，是会出现这种情况的。只需要调整数据的位置，不需要重新分配空间。
      new_start = map + (map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
      if (new_start < start.node)
        TinySTL::copy(start.node, finish.node + 1, new_start);
      else
        TinySTL::copy_backward(start.node, finish.node + 1, new_start + old_num_nodes);
    } else {
      size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
      map_pointer new_map = map_allocator::allocate(new_map_size);
      new_start = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
      TinySTL::copy(start.node, finish.node + 1, new_start);
      map_allocator::deallocate(map, map_size);
      map = new_map;
      map_size = new_map_size;
    }
    // 原来的 start.first 和 new_start.first 应该是一样的，所以这里没有变动。
    start.node = new_start;
    finish.node = new_start + old_num_nodes - 1;
  }
  const_iterator to_const_iterator(iterator it) const { return const_iterator(it.first, it.last, it.cur, it.node); }
};

}

#endif //TINYSTL_SRC_DEQUE_H_
