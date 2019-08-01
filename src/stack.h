#ifndef TINYSTL_SRC_STACK_H_
#define TINYSTL_SRC_STACK_H_

#include "deque.h"

namespace TinySTL {

template<typename T, typename Sequence = TinySTL::deque<T>>
class stack {
  friend bool operator==(const stack &lhs, const stack &rhs) { return lhs.c == rhs.c; }
  friend bool operator<(const stack &lhs, const stack &rhs) { return lhs.c < rhs.c; }
  friend bool operator!=(const stack &lhs, const stack &rhs) { return !(lhs == rhs); }
  friend void swap(stack &x, stack &y) { swap(x.c, y.c); }

 public:
  using value_type = typename Sequence::value_type;
  using size_type = typename Sequence::size_type;
  using reference = typename Sequence::reference;
  using const_reference = typename Sequence::const_reference;
 protected:
  Sequence c;

 public:
  stack() : c() {}
  explicit stack(const Sequence &_c) : c(_c) {}
  stack(const stack &val) : stack(val.c) {}
  stack(stack &&val) : stack() { swap(*this, val); }
  stack &operator=(stack val) {
    swap(*this, val);
    return *this;
  }

  bool empty() const { return c.empty(); }
  size_type size() const { return c.size(); }
  const_reference top() const { return c.back(); }

  reference top() { return c.back(); }
  void push(const value_type &val) { c.push_back(val); }
  void pop() { c.pop_back(); }
};

}

#endif //TINYSTL_SRC_STACK_H_
