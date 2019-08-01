#ifndef TINYSTL_SRC_QUEUE_H_
#define TINYSTL_SRC_QUEUE_H_

#include "deque.h"

namespace TinySTL {

template<typename T, typename Sequence = TinySTL::deque<T>>
class queue {
 public:
  friend bool operator==(const queue &lhs, const queue &rhs) { return lhs.c == rhs.c; }
  friend bool operator<(const queue &lhs, const queue &rhs) { return lhs.c < rhs.c; }
  friend bool operator!=(const queue &lhs, const queue &rhs) { return !(lhs == rhs); }
  friend void swap(queue &x, queue &y) { swap(x.c, y.c); }

 public:
  using value_type = typename Sequence::value_type;
  using size_type = typename Sequence::size_type;
  using reference = typename Sequence::reference;
  using const_reference = typename Sequence::const_reference;

 protected:
  Sequence c;

 public:
  queue() : c() {}
  explicit queue(const Sequence &seq) : c(seq) {}
  queue(const queue &val) : c(val.c) {}
  queue(queue &&val) : queue() { swap(*this, val); }
  queue &operator=(queue val) {
    swap(*this, val);
    return *this;
  }

  bool empty() const { return c.empty(); }
  size_type size() const { return c.size(); }
  reference front() { return c.front(); }
  const_reference front() const { return c.front(); }
  void push(const value_type &val) { c.push_back(val); }
  void pop() { c.pop_front(); }

};
}

#endif //TINYSTL_SRC_QUEUE_H_
