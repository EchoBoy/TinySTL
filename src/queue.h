#ifndef TINYSTL_SRC_QUEUE_H_
#define TINYSTL_SRC_QUEUE_H_
#include "deque.h"

namespace TinySTL {

template<typename T, typename Sequence = TinySTL::deque<T>>
class queue {
  friend bool operator==(const queue &x, const queue &y) { return x.c == y.c; }
  friend bool operator<(const queue &x, const queue &y) { return x.c < y.c; }
  friend bool operator!=(const queue &lhs, const queue &rhs) { return !(lhs == rhs); }
  friend void swap(queue &x, queue &y) { x.swap(y); }
 public:
  typedef typename Sequence::value_type value_type;
  typedef typename Sequence::size_type size_type;
  typedef typename Sequence::reference reference;
  typedef typename Sequence::const_reference const_reference;
 protected:
  Sequence c;
 public:
  queue() : c() {}
  explicit queue(const Sequence &seq) : c(seq) {}
  bool empty() const { return c.empty(); }
  size_type size() const { return c.size(); }
  reference front() { return c.front(); }
  const value_type &front() const { return c.front(); }
  void push(const value_type &val) { c.push_back(val); }
  void pop() { c.pop_front(); }
  void swap(queue &x) { c.swap(x.c); }

};
}

#endif //TINYSTL_SRC_QUEUE_H_
