#ifndef TINYSTL_SRC_DEQUE_H_
#define TINYSTL_SRC_DEQUE_H_

#include "allocator.h"
#include "iterator.h"
#include "uninitialized.h"
#include "algorithm.h"

namespace TinySTL {

// 默认一个 node 字节数
const static int __node_bytes = 1024;
const static int MiniMapSize = 8;

// ok_TODO: 这里没有考虑element_size 为零的情况. C++ 规定 class 或 struct 的sizeof最小结果为1。
static inline size_t __buf_size(size_t element_size) {
  return __node_bytes > element_size ? __node_bytes / element_size : 1;
}

template<typename T, typename Ref, typename Ptr>
struct __deque_iterator {
  typedef __deque_iterator<T, T &, T *> iterator;
  typedef __deque_iterator<T, const T &, const T *> const_iterator;
  static size_t buffer_size() { return __buf_size(sizeof(T)); }

  typedef random_iterator_tag iterator_category;
  typedef T value_type;
  typedef Ref reference;
  typedef Ptr pointer;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T **map_pointer;

  typedef __deque_iterator self_type;

 private:
  T *cur;
  T *first;
  T *last;
  map_pointer node;

 private:
  void set_node(map_pointer new_node) {
    node = new_node;
    first = *new_node;
    last = first + buffer_size();
  }
 public:
  __deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
  reference operator*() const { return *cur; }
  pointer operator->() const { return &(operator*()); }
  difference_type operator-(const self_type &x) const {
    if (node == nullptr || x.node == nullptr)
      return 0;
    return buffer_size() * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
  }
  self_type &operator++() {
    ++cur;
    if (cur == last) {
      set_node(node + 1);
      cur = first;
    }
    return *this;
  }
  const self_type operator++(int) {
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
  const self_type operator--(int) {
    auto res = cur;
    --(*this);
    return res;
  }
  self_type &operator+=(difference_type n) {
    if (n == 0) return *this;
    difference_type offset = n + (cur - first);
    if (offset >= 0 && offset < buffer_size())
      cur += n;
    else {
      difference_type node_offset =
          offset > 0 ?
          offset / buffer_size() :
          -difference_type((-offset - 1) / buffer_size()) - 1;
      set_node(node + node_offset);
      cur = first + (offset - node_offset * buffer_size());
    }
    return *this;
  }

  self_type operator+(difference_type n) const {
    self_type tmp = *this;
    return tmp += n;
  }
  self_type &operator-=(difference_type n) { return *this += -n; }
  self_type operator-(difference_type n) const {
    self_type tmp = *this;
    return tmp -= n;
  }

  bool operator==(const self_type &x) const { return cur == x.cur; }
  bool operator!=(const self_type &x) const { return !(*this == x); }
  bool operator<(const self_type &x) const {
    return (node == x.node) ? (cur < x.cur) : (node < x.node);
  }

  template<typename>
  friend
  class deque;
};

template<typename T>
class deque {
 public:
  typedef T value_type;
  typedef value_type *pointer;
  typedef size_t size_type;
  typedef __deque_iterator<T, T &, T *> iterator;
  typedef value_type &reference;
  typedef const value_type &const_reference;

 protected:
  typedef pointer *map_pointer;
  typedef allocator<value_type> node_allocator;
  typedef allocator<pointer> map_allocator;

 protected:
  iterator start;
  iterator finish;
  map_pointer map;
  size_type map_size;

 public:
  /*************** 构造、复制、赋值、析构相关 ************/
  deque() {
    create_map_and_nodes(1);
    start.cur = start.first;
    finish.cur = finish.first;
  }
  explicit deque(int n, const value_type &val = value_type()) { allocate_and_fill_n(n, val); }
  template<typename InputIterator>
  deque(InputIterator first, InputIterator last) { allocate_and_copy(first, last); }
  deque(const deque &x) { allocate_and_copy(x.begin(), x.end()); }
  ~deque() { destroy_and_deallocate_all(); }

  deque &operator=(const deque &x) {
    if (this != &x) {
      destroy_and_deallocate_all();
      allocate_and_copy(x.begin(), x.end());
    }
    return *this;
  }
  /*************** 比较相关 ************/
  bool operator==(const deque &x) const {
    auto b1 = begin(), b2 = x.begin();
    auto e1 = end(), e2 = x.end();
    for (; b1 != e1 && b2 != e2; ++b1, ++b2) {
      if (*b1 != *b2)
        return false;
    }
    return b1 == e1 && b2 == e2;
  }
  bool operator!=(const deque &x) const { return !operator==(x); }

  /*************** 迭代器相关 ************/
  iterator begin() { return start; }
  iterator end() { return finish; }
  // TODO： 还是没有解决 const_iterator 的问题
  iterator begin() const { return start; }
  iterator end() const { return finish; }

  /*************** 容量相关 ************/
  size_type size() const { return end() - begin(); }
  bool empty() const { return begin() == end(); }

  /*************** 访问元素相关 ************/
  reference operator[](size_type n) { return *(begin() + n); }
  reference front() { return *begin(); }
  reference back() { return *(end() - 1); }
  const_reference operator[](size_type n) const { return *(begin() + n); }
  const_reference front() const { return *begin(); }
  const_reference back() const { return *(end() - 1); }

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
  void swap(deque &x) {
    TinySTL::swap(start, x.start);
    TinySTL::swap(finish, x.finish);
    TinySTL::swap(map, x.map);
    TinySTL::swap(map_size, x.map_size);
  }
  // clear 的策略是保留一个缓冲区
  void clear() {
    for (auto cur = start.node + 1; cur < finish.node; ++cur) {
      node_allocator::destroy(*cur, (*cur) + buffer_size());
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

  /*************** 辅助函数 ************/
 protected:
  pointer new_node() { return node_allocator::allocate(buffer_size()); }
  void delete_node(pointer p) { node_allocator::deallocate(p, buffer_size()); }
  static size_t buffer_size() { return __buf_size(sizeof(value_type)); }
  // 本函数仅做初始化用途
  void create_map_and_nodes(size_type num_elements) {
    size_type num_nodes = num_elements / buffer_size() + 1;
    map_size = max(MiniMapSize, int(num_nodes + 2));
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
    finish.cur = finish.first + num_elements % buffer_size();
  }
  void allocate_and_fill_n(size_type n, const value_type &val) {
    create_map_and_nodes(n);
    for (auto cur = start.node; cur < finish.node; ++cur)
      TinySTL::uninitialized_fill(*cur, *cur + buffer_size(), val);
    TinySTL::uninitialized_fill(finish.first, finish.cur, val);
  }
  template<typename InputIterator>
  void allocate_and_copy(InputIterator first, InputIterator last) {
    create_map_and_nodes(distance(first, last));
    uninitialized_copy(first, last, start);
  }
  void destroy_and_deallocate_all() {
    if (!empty()) {
      node_allocator::destroy(start, finish);
      for (auto cur = start.node; cur != finish.node; ++cur)
        delete_node(*cur);
      delete_node(finish.first);
      map_allocator::deallocate(map);
    }
  }
  void push_back_aux(const value_type &val) {
    reserve_map_at_back();
    *(finish.node + 1) = new_node();
    node_allocator::construct(finish.cur, val);
    finish.set_node(finish.node + 1);
    finish.cur = finish.first;
  }
  void push_front_aux(const value_type &val) {
    reserve_amp_at_front();
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
  void reserve_amp_at_front(size_type nodes_to_add = 1) {
    if (nodes_to_add > start.node - map)
      reallocate_map(nodes_to_add, true);
  }
  void reallocate_map(size_type nodes_to_add, bool add_at_front) {
    size_type old_num_nodes = finish.node - start.node + 1;
    size_type new_num_nodes = old_num_nodes + nodes_to_add;

    map_pointer new_start;
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
};

}

#endif //TINYSTL_SRC_DEQUE_H_
