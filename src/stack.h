#ifndef TINYSTL_SRC_STACK_H_
#define TINYSTL_SRC_STACK_H_

#include "deque.h"

namespace TinySTL {

template<typename T, typename Sequence = TinySTL::deque<T>>
class stack {
  friend bool operator==(const stack &lhs, const stack &rhs) { return lhs.c == rhs.c; }
  friend bool operator<(const stack &lhs, const stack &rhs) { return lhs.c < rhs.c; }
  friend bool operator!=(const stack &lhs, const stack &rhs) { return !(lhs == rhs); }
  friend void swap(stack &lhs, stack &rhs) { lhs.swap(rhs); }

 public:
  typedef typename Sequence::value_type value_type;
  typedef typename Sequence::size_type size_type;
  typedef typename Sequence::reference reference;
 protected:
  Sequence c;

 public:
  stack() : c() {}
  explicit stack(const Sequence &c) : Sequence(c) {}
  bool empty() const { return c.empty(); }
  size_type size() const { return c.size(); }
  reference top() { return c.back(); }
  const value_type &top() const { return c.back(); }
  void push(const value_type &val) { c.push_back(val); }
  void pop() { c.pop_back(); }
  void swap(stack &x) { TinySTL::swap(c, x.c); }
};

}

#endif //TINYSTL_SRC_STACK_H_
