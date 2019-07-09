#ifndef TINYSTL_SRC_FUNCTIONAL_H_
#define TINYSTL_SRC_FUNCTIONAL_H_
namespace TinySTL {

template<typename T>
struct less {
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef bool result_type;
  result_type operator()(const first_argument_type &x, const second_argument_type &y) {
    return x < y;
  }
};

template<class T>
struct equal_to {
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef bool result_type;

  result_type operator()(const first_argument_type &x, const second_argument_type &y) {
    return x == y;
  }
};
}

#endif //TINYSTL_SRC_FUNCTIONAL_H_
