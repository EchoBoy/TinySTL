#ifndef TINYSTL_SRC_VECTOR_H_
#define TINYSTL_SRC_VECTOR_H_

#include "./allocator.h"
#include "./algorithm.h"
#include "./uninitialized.h"
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

  vector(const vector &v) { allocate_and_copy(v.begin(), v.end()); }
  vector &operator=(const vector &v) {
    if (this != &v) {
      destroy_and_deallocate_all();
      allocate_and_copy(v.begin(), v.end());
    }
    return *this;
  }

  ~vector() { destroy_and_deallocate_all(); }

  /*************** 比较相关 ************/
  bool operator==(const vector &v) const {
    if (size() != v.size())
      return false;
    auto it1 = begin(), last1 = end();
    auto it2 = v.begin(), last2 = v.end();
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
  iterator begin() { return start; }
  const_iterator begin() const { return start; }
  const_iterator cbegin() const { return start; }
  iterator end() { return finish; }
  const_iterator end() const { return finish; }
  const_iterator cend() const { return finish; }

  /*************** 容量相关 ************/
  size_type size() const { return size_type(finish - start); }
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
  void reserve(size_type n) {
    if (n <= capacity()) return;
    iterator new_start = data_allocator::allocate(n);
    iterator new_finish = TinySTL::uninitialized_copy(begin(), end(), new_start);
    destroy_and_deallocate_all();
    start = new_start;
    finish = new_finish;
    end_of_storage = new_start + n;
  }

  /*************** 访问元素相关 ************/
  reference operator[](const size_type i) { return *(begin() + i); }
  const_iterator operator[](const size_type i) const { return *(cbegin() + i); }
  reference front() { return *(begin()); }
  reference back() { return *(end() - 1); }
  pointer data() { return start; }

  /*************** 相关容器相关 ************/
  void insert(iterator position, const value_type &val) { insert(position, 1, val); }
  void insert(iterator position, size_type n, const value_type &val) {
    typedef typename __type_traits<size_type>::is_integer is_integer;
    insert_aux(position, n, val, is_integer());
  }
  template<typename InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last) {
    typedef typename __type_traits<InputIterator>::is_integer is_integer;
    insert_aux(position, first, last, is_integer());
  }
  void clear() {
    destroy(start, finish);
    finish = start;
  }
  void swap(vector &v) {
    if (this != &v) {
      TinySTL::swap(start, v.start);
      TinySTL::swap(finish, v.finish);
      TinySTL::swap(end_of_storage, v.end_of_storage);
    }
  }
  void push_back(const value_type &val) { insert(end(), val); }
  void pop_back() {
    --finish;
    data_allocator::destroy(finish);
  }
  iterator erase(iterator first, iterator last) {
    iterator new_it = first, old_it = last;
    for (; old_it != finish; ++old_it, ++new_it)
      *new_it = *old_it;
    finish -= last - first;
    return first;
  }
  iterator erase(iterator position) { return erase(position, position + 1); }


  /*************** 辅助函数 ************/
 protected:
  size_type left_storage() { return size_type(end_of_storage - finish); }
  void destroy_and_deallocate_all() {
    if (capacity() != 0) {
      data_allocator::destroy(start, finish);
      data_allocator::deallocate(start, capacity());
    }
  }
  void reallocate_and_fill_n(iterator fill_position, size_type n, const value_type &val) {
    size_type new_cap = get_new_capacity(n);

    iterator new_start = data_allocator::allocate(new_cap);
    iterator new_end_of_storage = new_start + new_cap;
    iterator new_finish = TinySTL::uninitialized_copy(begin(), fill_position, new_start);
    new_finish = TinySTL::uninitialized_fill_n(new_finish, n, val);
    new_finish = TinySTL::uninitialized_copy(fill_position, end(), new_finish);

    destroy_and_deallocate_all();

    start = new_start;
    finish = new_finish;
    end_of_storage = new_end_of_storage;
  }
  template<typename InputIterator>
  void reallocate_and_copy(iterator fill_position, InputIterator first, InputIterator last) {
    auto need_storage = distance(first, last);
    auto new_cap = get_new_capacity(need_storage);

    iterator new_start = data_allocator::allocate(new_cap);
    iterator new_end_of_storage = new_start + new_cap;
    iterator new_finish = TinySTL::uninitialized_copy(begin(), fill_position, new_start);
    new_finish = TinySTL::uninitialized_copy(first, last, new_finish);
    new_finish = TinySTL::uninitialized_copy(fill_position, end(), new_finish);

    destroy_and_deallocate_all();

    start = new_start;
    finish = new_finish;
    end_of_storage = new_end_of_storage;
  }
  template<typename InputIterator>
  void allocate_and_copy(InputIterator first, InputIterator last) {
    start = data_allocator::allocate(TinySTL::distance(first, last));
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
  template <typename Integer>
  void insert_aux(iterator position, Integer n, const value_type &val, __true_type);
  template<typename InputIterator>
  void insert_aux(iterator position, InputIterator first, InputIterator last, __false_type);
};

template<typename T, typename Alloc>
template <typename Integer>
void vector<T, Alloc>::insert_aux(iterator position, Integer n, const value_type &val, __true_type) {
  if (n == 0) return;

  if (left_storage() >= n) {
    // 剩下空间够用
    auto new_finish = finish + n;
    TinySTL::copy_backward(position, finish, new_finish);
    TinySTL::uninitialized_fill_n(position, n, val);
    finish = new_finish;
  } else {
    // 空间不够用，重新分配
    reallocate_and_fill_n(position, n, val);
  }
}
template<typename T, typename Alloc>
template<typename InputIterator>
void vector<T, Alloc>::insert_aux(iterator position, InputIterator first, InputIterator last, __false_type) {
  auto storage_need = distance(first, last);
  if (left_storage() >= storage_need) {
    auto new_finish = finish + storage_need;
    // TODO：这里调用 copy 是否合理？copy是基于赋值的，当如果目的地没有初始化（构造）如何赋值？
    TinySTL::copy_backward(position, finish, new_finish);
    TinySTL::uninitialized_copy(first, last, position);
    finish = new_finish;
  } else {
    reallocate_and_copy(position, first, last);
  }
}

}

#endif //TINYSTL_SRC_VECTOR_H_
