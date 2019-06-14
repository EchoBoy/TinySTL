#ifndef TINYSTL_SRC_ITERATOR_H_
#define TINYSTL_SRC_ITERATOR_H_

#include <stddef.h>
namespace TinySTL {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_iterator_tag : public bidirectional_iterator_tag {};

template<typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T *, typename Reference = T &>
struct iterator {
  typedef Category iterator_category;
  typedef T value_type;
  typedef Distance difference_type;
  typedef Pointer pointer;
  typedef Reference reference;
};

// 榨汁机!!!!!！封装一层是为了兼容 native pointer
template<typename Iterator>
struct iterator_traits {
  typedef typename Iterator::iterator_category iterator_category;
  typedef typename Iterator::value_type value_type;
  typedef typename Iterator::difference_type difference_type;
  typedef typename Iterator::pointer pointer;
  typedef typename Iterator::reference reference;
};

// native pinter 兼容
template<typename T>
struct iterator_traits<T *> {
  typedef random_iterator_tag iterator_category;
  typedef T value_type;
  typedef ptrdiff_t difference_type;
  typedef T *pointer;
  typedef T &reference;
};

// const native pinter 兼容
template<typename T>
struct iterator_traits<const T *> {
  typedef random_iterator_tag iterator_category;
  typedef T value_type;
  typedef ptrdiff_t difference_type;
  typedef const T *pointer;
  typedef const T &reference;
};

template<typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator &) {
  typedef typename iterator_traits<Iterator>::iterator_category category;
  return category();
}

template<typename Iterator>
inline typename iterator_traits<Iterator>::difference_type *
distance_type(const Iterator &) {
  return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
}

template<typename Iterator>
inline typename iterator_traits<Iterator>::value_type *
value_type(const Iterator &) {
  return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
}

}

#endif //TINYSTL_SRC_ITERATOR_H_
