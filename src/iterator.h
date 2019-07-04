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

/***************** [distance] T(n) = O(n) *********************/
template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag) {
  typename iterator_traits<InputIterator>::difference_type dist = 0;
  for (; first != last; ++first, ++dist);
  return dist;
}

template<typename RandomIterator>
inline typename iterator_traits<RandomIterator>::difference_type
__distance(RandomIterator first, RandomIterator last, random_iterator_tag) {
  return last - first;
}

template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
  typedef typename iterator_traits<InputIterator>::iterator_category i_c;
  return __distance(first, last, i_c());
}

/***************** [advance] T(n) = O(n) *********************/
template<typename InputIterator, typename Distance>
void
__advance(InputIterator &it, Distance n, input_iterator_tag) {
  assert(n >= 0);
  for (; n != 0; --n, ++it);
}
template<typename BidirectionalIterator, typename Distance>
void
__advance(BidirectionalIterator &it, Distance n, bidirectional_iterator_tag) {
  if (n < 0) {
    for (; n != 0; ++n, --it);
  } else {
    for (; n != 0; --n, ++it);
  }
}
template<typename RandomIterator, typename Distance>
inline void
__advance(RandomIterator &it, Distance n, random_iterator_tag) {
  if (n < 0) {
    it -= (-n);
  } else {
    it += n;
  }
}
template<typename InputIterator, typename Distance>
inline void
advance(InputIterator &it, Distance n) {
  typedef typename iterator_traits<InputIterator>::iterator_category i_c;
  __advance(it, n, i_c());
}

}

#endif //TINYSTL_SRC_ITERATOR_H_
