#ifndef TINYSTL_SRC_VECTOR_H_
#define TINYSTL_SRC_VECTOR_H_

#include "./allocator.h"
#include "./algorithm.h"
#include "./uninitialized.h"
#include "./construct.h"
#include <stddef.h> // for size_t, ptrdiff_t

namespace TinySTL {

template<typename T, typename Alloc=allocator<T>>
class vector {
 public:
  typedef T value_type;
  typedef value_type *pointer;
  typedef value_type *iterator;
  typedef const value_type *const_iterator;
  typedef value_type &reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

 protected:
  typedef Alloc data_allocator;
  iterator start;
  iterator finish;
  iterator end_of_storage;

 public:
  /*************** 构造、复制、析构相关 ************/
  vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}
  template<typename InputIterator>
  vector(InputIterator first, InputIterator last) {
    allocate_and_copy(first, last);
  }
  // 处理指针和数字间的区别
  vector(int n, int val) {
    allocate_and_fill_n(n, val);
  }
  explicit vector(size_type n) { allocate_and_fill_n(n, value_type()); }
  vector(size_type n, const value_type &value) { allocate_and_fill_n(n, value); }
  ~vector() { destroy_and_deallocate_all(); }

  /*************** 比较相关 ************/
  bool operator==(const vector &v) const {
    if (size() != v.size())
      return false;
    auto it1 = start, last1 = finish;
    auto it2 = v.start, last2 = v.finish;
    for (; it1 != last1 && it2 != last2; ++it1, ++it2) {
      if (*it1 != *it2)
        return false;
    }
    return true;
  }
  bool operator!=(const vector &v) const {
    return !(*this == v);
  }

  /*************** 迭代器相关 ************/
  iterator begin() const { return start; }
  iterator end() const { return finish; }

  /*************** 容量相关 ************/
  size_type size() const { return size_type(end() - begin()); }
  size_type capacity() const { return size_type(end_of_storage - start); }
  bool empty() const { return begin() == end(); }
  void resize(size_type new_size, const value_type &x) {
    if (new_size < size()) {
      erase(begin() + new_size, end());
    } else {
      insert(end(), new_size - size(), x);
    }
  }
  void resize(size_type new_size) { resize(new_size, T()); }

  /*************** 访问元素相关 ************/
  reference operator[](size_type i) { return *(begin() + i); }
  reference front() { return *(begin()); }
  reference back() { return *(end() - 1); }

  /*************** 相关容器相关 ************/
  void insert(iterator position, const value_type &val) { insert(position, 1, val); }
  void insert(iterator position, size_type n, const value_type &val);
//  void clear();
//  void swap(vector &v);
  void push_back(const value_type &val) { insert(end(), val); }
//  void pop_back();
//  iterator erase(iterator position);

  // 辅助函数
 protected:
  void destroy_and_deallocate_all() {
    if (capacity() != 0) {
      TinySTL::destroy(start, finish);
      data_allocator::deallocate(start, capacity());
    }
  }
  void reallocate_and_fill_n(iterator fill_position, size_type n, const value_type &val) {
    size_type new_cap = get_new_capacity(n);

    iterator new_start = data_allocator::allocate(new_cap);
    iterator new_end_of_storage = new_start + new_cap;
    iterator new_finish = uninitialized_copy(begin(), fill_position, new_start);
    new_finish = uninitialized_fill_n(new_finish, n, val);
    new_finish = uninitialized_copy(fill_position, end(), new_finish);

    destroy_and_deallocate_all();

    start = new_start;
    finish = new_finish;
    end_of_storage = new_end_of_storage;
  }
  template<typename InputIterator>
  void allocate_and_copy(InputIterator first, InputIterator last) {
    start = data_allocator::allocate(last - first);
    finish = TinySTL::uninitialized_copy(first, last, start);
    end_of_storage = finish;
  }
  void allocate_and_fill_n(size_type n, const value_type &val) {
    start = data_allocator::allocate(n);
    TinySTL::uninitialized_fill_n(start, n, val);
    finish = start + n;
    end_of_storage = finish;
  }
  size_type get_new_capacity(size_type extra_n) const {
    size_type old_cap = capacity();
    return old_cap + max(old_cap, extra_n);
  }
};

template<typename T, typename Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const value_type &val) {
  if (n == 0) return;

  if (size_type(end_of_storage - finish) >= n) {
    // 剩下空间够用
    auto new_finish = finish + n;
    copy_backward(position, finish, new_finish);
    uninitialized_fill_n(position, n, val);
    finish = new_finish;
  } else {
    // 空间不够用，重新分配
    reallocate_and_fill_n(position, n, val);
  }
}

}

#endif //TINYSTL_SRC_VECTOR_H_
