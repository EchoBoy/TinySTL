#ifndef TINYSTL_SRC_LIST_H_
#define TINYSTL_SRC_LIST_H_

#include "algorithm.h"
#include "allocator.h"
#include "iterator.h"
#include "functional.h" // for less<T>

namespace TinySTL {

template<typename T>
struct __list_node {
  // 成员变量私有，通过定义 friend class 来访问
  template<typename, typename, typename>
  friend
  class __list_iterator;
  template<typename, typename>
  friend
  class list;

 private:
  T data;
  __list_node *prev;
  __list_node *next;

 public:
  __list_node(const T &d, __list_node *p, __list_node *n) : data(d), prev(p), next(n) {}
  friend bool operator==(const __list_node &lhs, const __list_node &rhs) {
    return lhs.data = rhs.data && lhs.prev == rhs.prev && lhs.next == rhs.next;
  }
};

template<typename T, typename Ref, typename Ptr>
struct __list_iterator : public iterator<bidirectional_iterator_tag, T> {
  using value_type = T;
  using reference = Ref;
  using pointer = Ptr;
  using self_type = __list_iterator<T, Ref, Ptr>;
  using node_ptr = __list_node<T> *;

  template<typename T1, typename Alloc>
  friend
  class list;

 private:
  node_ptr ptr;

 public:
  __list_iterator() : ptr(nullptr) {}
  explicit __list_iterator(node_ptr p) : ptr(p) {}
  __list_iterator(const self_type &val) : ptr(val.ptr) {}

  friend bool operator==(const self_type &lhs, const self_type &rhs) { return lhs.ptr == rhs.ptr; }
  friend bool operator!=(const self_type &lhs, const self_type &rhs) { return lhs.ptr != rhs.ptr; }
  reference operator*() { return ptr->data; }
  pointer operator->() { return &(operator*()); }
  self_type &operator++() {
    ptr = ptr->next;
    return *this;
  }
  self_type &operator--() {
    ptr = ptr->prev;
    return *this;
  }
  self_type operator++(int) {
    self_type tmp = *this;
    ++(*this);
    return tmp;
  }
  self_type operator--(int) {
    self_type tmp = *this;
    --(*this);
    return tmp;
  }
};
template<typename T, typename Alloc=allocator<__list_node<T>>>
class list {
 protected:
  using node = __list_node<T>;
  using const_node = __list_node<const T>;
  using node_ptr = node *;
  using const_node_ptr =  const_node *;

 public:
  using value_type = T;
  using iterator = __list_iterator<T, T &, T *>;
  using const_iterator = __list_iterator<T, const T &, const T *>;
  using reference = T &;
  using const_reference  = const T &;
  using size_type = size_t;

 protected:
  using data_allocator = Alloc;
  // 使用循环双向链表
  node_ptr dumpy_head;

 public:
  /**** 生命周期：ctor、copy ctor、copy assignment、move ctor、move assignment、dtor ****/
  list() { init_dumpy_head(); };
  explicit list(size_type n, const value_type &val = value_type()) {
    typedef typename __type_traits<size_type>::is_integer is_integer;
    ctor_aux(n, val, is_integer());
  }
  template<typename InputIterator>
  list(InputIterator first, InputIterator last) {
    typedef typename __type_traits<InputIterator>::is_integer is_integer;
    ctor_aux(first, last, is_integer());
  }
  // Rule of five
  list(const list &l) : list(l.begin(), l.end()) {}
  list(list &&x) : list() { swap(*this, x); }
  list &operator=(list l) {
    swap(*this, l);
    return *this;
  }
  ~list() {
    auto p = dumpy_head->next;
    auto next_p = p->next;
    for (; p != dumpy_head; p = next_p) {
      next_p = p->next;
      delete_node(p);
    }
    delete_node(dumpy_head);
  }
  /*************** public const member functions ************/
  const_iterator begin() const { return const_iterator(dumpy_head->next); }
  const_iterator end() const { return const_iterator(dumpy_head); }
  size_type size() const { return static_cast<size_type>(TinySTL::distance(begin(), end())); }
  bool empty() const { return dumpy_head->next == dumpy_head; }

  /*************** public member functions ************/
  iterator begin() { return iterator(dumpy_head->next); }
  iterator end() { return iterator(dumpy_head); }

  /*************** 访问元素相关 ************/
  reference front() { return dumpy_head->next->data; }
  reference back() { return dumpy_head->prev->data; }

  /*************** 插入、删除相关 ************/
  void insert(iterator position, const value_type &val) { __insert(position.ptr, val); }
  void insert(iterator position, size_type n, const value_type &val) {
    typedef typename __type_traits<size_type>::is_integer is_integer;
    insert_aux(position, n, val, is_integer());
  }
  template<typename InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last) {
    typedef typename __type_traits<InputIterator>::is_integer is_integer;
    insert_aux(position, first, last, is_integer());
  }
  void push_front(const value_type &val) { __insert(dumpy_head->next, val); }
  void pop_front() { __erase(dumpy_head->next); }
  void push_back(const value_type &val) { __insert(dumpy_head, val); }
  void pop_back() { __erase(dumpy_head->prev); }

  iterator erase(iterator position) { return iterator(__erase(position.ptr)); }
  iterator erase(iterator first, iterator last) { return iterator(__erase(first.ptr, last.ptr)); }

  void clear() { erase(begin(), end()); }

  void remove(const value_type &val) {
    auto it = begin();
    auto last = end();
    while (it != last) {
      it = (*it == val ? erase(it) : ++it);
    }
  }
  template<typename Predicate>
  void remove_if(Predicate pred) {
    auto it = begin();
    auto last = end();
    while (it != last) {
      it = pred(*it) ? erase(it) : ++it;
    }
  }
  void splice(iterator position, list &x) {
    splice(position, x, x.begin(), x.end());
  }
  void splice(iterator position, list &x, iterator it) {
    auto last = it;
    ++last;
    splice(position, x, it, last);
  }
  void splice(iterator position, list &, iterator first, iterator last) {
    node_ptr curr = first.ptr;
    node_ptr prev = first.ptr->prev;
    node_ptr next = last.ptr;
    transfer(position.ptr, curr, next);
    prev->next = next;
    next->prev = prev;
  }

  template<typename Compare>
  void merge(list &x, Compare comp) {
    auto it1 = begin();
    auto it2 = x.begin();
    auto last1 = end();
    auto last2 = x.end();
    while (it1 != last1 && it2 != last2) {
      if (comp(*it1, *it2)) {
        ++it1;
      } else {
        auto tmp = it2;
        ++it2;
        transfer(it1.ptr, tmp.ptr, it2.ptr);
      }
    }
    if (it2 != last2)
      transfer(last1.ptr, it2.ptr, last2.ptr);
  }
  void merge(list &x) { merge(x, TinySTL::less<T>()); }

  void reverse() {
    if (dumpy_head->next->next == dumpy_head)
      return;
    iterator it = begin();
    ++it;
    while (it != end()) {
      auto tmp = it;
      ++it;
      transfer(begin().ptr, tmp.ptr, it.ptr);
    }
  }

  template<typename Compare>
  void sort(Compare comp) {
    if (dumpy_head->next->next == dumpy_head)
      return;
    list<T> carry;
    list<T> counter[64];
    int fill = 0;
    while (!empty()) {
      auto size = this->size();
      carry.splice(carry.begin(), *this, begin());
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        counter[i].merge(carry, comp);
        swap(carry, counter[i++]);
      }
      swap(carry, counter[i]);
      if (i == fill)
        ++fill;
    }
    for (int i = 1; i < fill; ++i)
      counter[i].merge(counter[i - 1], comp);
    swap(*this, counter[fill - 1]);
  }
  void sort() { sort(TinySTL::less<T>()); }
 public:
  /*************** 我的朋友 ************/
  friend void swap(list &x, list &y) {
    using TinySTL::swap;
    swap(x.dumpy_head, y.dumpy_head);
  }

  /*************** 辅助函数 ************/
 protected:
  void init_dumpy_head() {
    dumpy_head = new_node();
    dumpy_head->prev = dumpy_head;
    dumpy_head->next = dumpy_head;
  }
  void ctor_aux(size_type n, const value_type &val, __true_type) {
    init_dumpy_head();
    for (; n > 0; --n)
      push_back(val);
  }
  template<typename InputIterator>
  void ctor_aux(InputIterator first, InputIterator last, __false_type) {
    init_dumpy_head();
    for (; first != last; ++first)
      push_back(*first);
  }
  void insert_aux(iterator position, size_type n, const value_type &val, __true_type) {
    for (; n > 0; --n)
      insert(position, val);
  }
  template<typename InputIterator>
  void insert_aux(iterator position, InputIterator first, InputIterator last, __false_type) {
    for (; first != last; ++first)
      insert(position, *first);
  }

 protected:
  node_ptr new_node(const T &val = T(), node_ptr pre = nullptr, node_ptr next = nullptr) {
    node_ptr res = data_allocator::allocate();
    data_allocator::construct(res, node(val, pre, next));
    return res;
  }
  void delete_node(node_ptr p) {
    data_allocator::destroy(&(p->data));
    data_allocator::deallocate(p);
  }
  void __insert(node_ptr position, const value_type &val) {
    auto tmp_node = new_node(val, position->prev, position);
    position->prev->next = tmp_node;
    position->prev = tmp_node;
  }
  node_ptr __erase(node_ptr position) {
    position->prev->next = position->next;
    position->next->prev = position->prev;
    auto res = position->next;
    delete_node(position);
    return res;
  }
  node_ptr __erase(node_ptr first, node_ptr last) {
    first->prev->next = last;
    last->prev = first->prev;
    auto next = first->next;
    while (first != last) {
      delete_node(first);
      first = next;
      next = next->next;
    }
    return last;
  }
  void transfer(node_ptr position, node_ptr first, node_ptr last) {
    if (position != last) {
      last->prev->next = position;
      first->prev->next = last;
      position->prev->next = first;
      auto tmp = position->prev;
      position->prev = last->prev;
      last->prev = first->prev;
      first->prev = tmp;
    }
  }
};

}

#endif //TINYSTL_SRC_LIST_H_
