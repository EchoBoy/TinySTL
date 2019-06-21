#ifndef TINYSTL_TEST_TEST_UTILS_H_
#define TINYSTL_TEST_TEST_UTILS_H_

namespace TinySTL {
namespace Test {

template<typename Container1, typename Container2>
bool container_equal(Container1 &con1, Container2 &con2) {
  auto first1 = con1.begin(), last1 = con1.end();
  auto first2 = con2.begin(), last2 = con2.end();
  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
    if (*first1 != *first2)
      return false;
  }
  return first1 == last1 && first2 == last2;
}
}
}

#endif //TINYSTL_TEST_TEST_UTILS_H_
