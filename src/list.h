#ifndef TINYSTL_SRC_LIST_H_
#define TINYSTL_SRC_LIST_H_

#include "algorithm.h"
#include "allocator.h"
#include "iterator.h"
#include "functional.h" // for less<T>

namespace TinySTL {

template<typename T>
struct __list_node {
  T data;
  __list_node *prev;
  __list_node *next;
  __list_node(const T &d, __list_node *p, __list_node *n) : data(d), prev(p), next(n) {}
  bool operator==(const __list_node &n) {
    return data = n.data && prev == n.prev && next == n.next;
  }
};

template<typename T>
struct __list_iterator : public iterator<bidirectional_iterator_tag, T> {
  typedef __list_iterator<T> self_type;
  typedef __list_node<T> *node_ptr;

  node_ptr ptr;

  __list_iterator() : ptr(nullptr) {}
  explicit __list_iterator(node_ptr p) : ptr(p) {}
  __list_iterator(const self_type &val) : ptr(val.ptr) {}

  bool operator==(const self_type &val) { return ptr == val.ptr; }
  bool operator!=(const self_type &val) { return ptr != val.ptr; }
  T &operator*() { return ptr->data; }
  T *operator->() { return &(operator*()); }
  self_type &operator++() {
    ptr = ptr->next;
    return *this;
  }
  self_type &operator--() {
    ptr = ptr->prev;
    return *this;
  }
  const self_type operator++(int) {
    self_type tmp = *this;
    ++(*this);
    return tmp;
  }
  const self_type operator--(int) {
    self_type tmp = *this;
    --(*this);
    return tmp;
  }
};
template<typename T, typename Alloc=allocator<__list_node<T>>>
class list {
 protected:
  typedef __list_node<T> node;
  typedef __list_node<const T> const_node;
  typedef node *node_ptr;
  typedef const_node *const_node_ptr;

 public:
  typedef T value_type;
  typedef __list_iterator<T> iterator;
  typedef __list_iterator<const T> const_iterator;
  typedef T &reference;
  typedef size_t size_type;

 protected:
  typedef Alloc data_allocator;
  node_ptr dumpy_head;

 public:
  /*************** 构造、复制、赋值、析构相关 ************/
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
  list(const list &l) { ctor_aux(l.begin(), l.end(), __false_type()); }
  list &operator=(const list &l) {
    if (this != &l) {
      list<T>(l).swap(*this);
    }
    return *this;
  }
  ~list() {
    for (auto p = dumpy_head->next; p != dumpy_head; p = p->next)
      delete_node(p);
    delete_node(dumpy_head);
  }
  /*************** 迭代器相关 ************/
  // TODO：这里是一个错误的做法。不应该将一下两函数标记为const
  iterator begin() const { return __list_iterator<T>(dumpy_head->next); }
  iterator end() const { return __list_iterator<T>(dumpy_head); }
//  const_iterator begin() const { return node_ptr_to_const_it(dumpy_head->next); }
//  const_iterator end() const { return node_ptr_to_const_it(dumpy_head); }

  /*************** 容量相关 ************/
  bool empty() const { return dumpy_head->next == dumpy_head; }
  size_type size() const { return static_cast<size_type>(TinySTL::distance(begin(), end())); }

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

  iterator erase(iterator position) { return __list_iterator<T>(__erase(position.ptr)); }
  iterator erase(iterator first, iterator last) { return __list_iterator<T>(__erase(first.ptr, last.ptr)); }

  void swap(list &val) {
    if (this != &val) {
      TinySTL::swap(dumpy_head, val.dumpy_head);
    }
  }
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
        carry.swap(counter[i++]);
      }
      carry.swap(counter[i]);
      if (i == fill)
        ++fill;
    }
    for (int i = 1; i < fill; ++i)
      counter[i].merge(counter[i - 1], comp);
    swap(counter[fill - 1]);
  }
  void sort() { sort(TinySTL::less<T>()); }



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
  // TODO: 取tmp的地址，tmp是一个stack-based的变量，应该不能取地址返回才对
//  const_iterator node_ptr_to_const_it(node_ptr p) const {
//    const_node tmp(p->data, (const_node_ptr) p->prev, (const_node_ptr) p->next);
//    return const_iterator(&tmp);
//  }

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
