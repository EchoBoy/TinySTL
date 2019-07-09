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
  /*************** 构造、复制、赋值、析构相关 ************/
  vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}
  template<typename InputIterator>
  vector(InputIterator first, InputIterator last) { allocate_and_copy(first, last); }
  // TODO：处理指针和数字间的区别
  vector(int n, int val) { allocate_and_fill_n(n, val); }
  // TODO: std 的做法是调用 type_value 的构造函数n次，我这里是构造一个tmp，再调用 n 次拷贝构造
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
  bool operator!=(const vector &v) const { return !(*this == v); }

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

  /*************** 插入删除操作相关 ************/
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
    data_allocator::destroy(start, finish);
    finish = start;
  }
  void swap(vector &v) {
    if (this != &v) {
      TinySTL::swap(start, v.start);
      TinySTL::swap(finish, v.finish);
      TinySTL::swap(end_of_storage, v.end_of_storage);
    }
  }
  void push_back(const value_type &val) {
    if (finish != end_of_storage) {
      data_allocator::construct(finish, val);
      ++finish;
    } else {
      insert(end(), val);
    }
  }
  void pop_back() {
    --finish;
    data_allocator::destroy(finish);
  }
  /// 举例：1，2，3，删除 1：2-> 1, 3->2, 析构3
  iterator erase(iterator first, iterator last) {
    // 需要移动 last 到 finish 之间的元素
    if (last != finish) {
      TinySTL::copy(last, finish, first);
    }
    auto new_finish = finish - (last - first);
    data_allocator::destroy(new_finish, finish);
    finish = new_finish;
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
    auto need_storage = TinySTL::distance(first, last);
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
  template<typename Integer>
  void insert_aux(iterator position, Integer n, const value_type &val, __true_type);
  template<typename InputIterator>
  void insert_aux(iterator position, InputIterator first, InputIterator last, __false_type);
};

template<typename T, typename Alloc>
template<typename Integer>
void vector<T, Alloc>::insert_aux(iterator position, Integer n, const value_type &val, __true_type) {
  if (n == 0) return;

  if (left_storage() >= n) {
    // 剩下空间够用
    auto need_move_size = finish - position;
    auto new_finish = finish + n;
    if (need_move_size > n) {
      TinySTL::uninitialized_copy(finish - n, finish, finish);
      TinySTL::copy_backward(position, finish - n, finish);
      TinySTL::fill_n(position, n, val);
    } else {
      TinySTL::uninitialized_fill_n(finish, n - need_move_size, val);
      TinySTL::uninitialized_copy(position, finish, finish + (n - need_move_size));
      TinySTL::fill_n(position, need_move_size, val);
    }
    finish = new_finish;
  } else {
    // 空间不够用，重新分配
    reallocate_and_fill_n(position, n, val);
  }
}
template<typename T, typename Alloc>
template<typename InputIterator>
void vector<T, Alloc>::insert_aux(iterator position, InputIterator first, InputIterator last, __false_type) {
  auto storage_need = TinySTL::distance(first, last);
  if (left_storage() >= storage_need) {
    auto need_move_size = finish - position;
    auto new_finish = finish + storage_need;
    if (need_move_size > storage_need) {
      TinySTL::uninitialized_copy(finish - storage_need, finish, finish);
      TinySTL::copy_backward(position, finish - storage_need, finish);
      TinySTL::copy(first, last, position);
    } else {
      TinySTL::uninitialized_copy(last - (storage_need - need_move_size), last, finish);
      TinySTL::uninitialized_copy(position, finish, finish + (storage_need - need_move_size));
      TinySTL::copy(first, last - (storage_need - need_move_size), position);
    }
    finish = new_finish;
  } else {
    reallocate_and_copy(position, first, last);
  }
}

}

#endif //TINYSTL_SRC_VECTOR_H_
