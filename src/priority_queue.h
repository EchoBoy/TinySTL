#ifndef TINYSTL_SRC_PRIORITY_QUEUE_H_
#define TINYSTL_SRC_PRIORITY_QUEUE_H_

#include "vector.h"
#include "functional.h"
#include "algorithm.h"

namespace TinySTL {
template<typename T, typename Sequence = vector<T>, typename Compare = less<typename Sequence::value_type>>
class priority_queue {
 public:
  using value_type = typename Sequence::value_type;
  using size_type = typename Sequence::size_type;
  using reference = typename Sequence::reference;
  using const_reference = typename Sequence::const_reference;

 protected:
  Sequence c;
  Compare comp;

 public:
  /// 生命周期
  priority_queue() : c() {}
  explicit priority_queue(const Compare &x) : c(), comp(x) {}
  template<typename InputIterator>
  priority_queue(InputIterator first, InputIterator last, const Compare &x): c(first, last), comp(x) {
    make_heap(c.begin(), c.end(), comp);
  }
  template<typename InputIterator>
  priority_queue(InputIterator first, InputIterator last): c(first, last), comp() {
    TinySTL::make_heap(c.begin(), c.end(), comp);
  }
  priority_queue(const priority_queue &val) : priority_queue(val.c.begin(), val.c.end(), val.comp) {}
  priority_queue(priority_queue &&val) : priority_queue() { swap(*this, val); }
  priority_queue &operator=(priority_queue val) {
    swap(*this, val);
    return *this;
  }

  bool empty() const { return c.empty(); }
  size_type size() const { return c.size(); }
  const_reference top() const { return c.front(); }

  /// 会修改容器的成员函数
  void push(const value_type &val) {
    c.push_back(val);
    TinySTL::push_heap(c.begin(), c.end(), comp);
  }
  void pop() {
    TinySTL::pop_heap(c.begin(), c.end(), comp);
    c.pop_back();
  }
 public:
  friend void swap(priority_queue &x, priority_queue &y) {
    TinySTL::swap(x.c, y.c);
    TinySTL::swap(x.comp, y.comp);
  }
};
}

#endif //TINYSTL_SRC_PRIORITY_QUEUE_H_
