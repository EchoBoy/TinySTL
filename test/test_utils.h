#ifndef TINYSTL_TEST_TEST_UTILS_H_
#define TINYSTL_TEST_TEST_UTILS_H_
#include <iostream>
#include <string>
using std::cout;
using std::endl;

namespace TinySTL {
namespace Test {

template<typename Container1, typename Container2>
bool container_equal(const Container1 &con1, const Container2 &con2) {
  auto first1 = con1.begin(), last1 = con1.end();
  auto first2 = con2.begin(), last2 = con2.end();
  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
    if (*first1 != *first2)
      return false;
  }
  return first1 == last1 && first2 == last2;
}

// 模拟常用的Class，方便测试容器是否正确调用元素的构造、拷贝构造、赋值运算符、析构。
class CountLife {
 private:
  static int ctor_cnt;
  static int dtor_cnt;
  static int copy_ctor_cnt;
  static int copy_assignment_cnt;

 private:
  int *data;
 public:
  // default constructor
  CountLife() {
    ++ctor_cnt;

    data = new int;
    *data = ctor_cnt;
  }
  // copy constructor
  CountLife(const CountLife &val) {
    ++copy_ctor_cnt;

    data = new int;
    *data = *(val.data);
  }
  // copy assignment_cnt;
  CountLife &operator=(const CountLife &val) {
    ++copy_assignment_cnt;

    *data = *(val.data);
    return *this;
  }
  // destructor
  ~CountLife() {
    ++dtor_cnt;

    delete data;
  }
  static void set_zero_all() {
    ctor_cnt = 0;
    dtor_cnt = 0;
    copy_ctor_cnt = 0;
    copy_assignment_cnt = 0;
  }
  static int get_ctor_cnt() { return ctor_cnt; }
  static int get_dtor_cnt() { return dtor_cnt; }
  static int get_copy_ctor_cnt() { return copy_ctor_cnt; }
  static int get_copy_assignment_cnt() { return copy_assignment_cnt; }
  static std::string to_string() {
    std::string res;
    return "ctor_cnt: " + std::to_string(ctor_cnt)
        + "\ndtor_cnt: " + std::to_string(dtor_cnt)
        + "\ncopy_ctor_cnt: " + std::to_string(copy_ctor_cnt)
        + "\ncopy_assignment_cnt: " + std::to_string(copy_assignment_cnt);
  }
};
int CountLife::ctor_cnt = 0;
int CountLife::dtor_cnt = 0;
int CountLife::copy_ctor_cnt = 0;
int CountLife::copy_assignment_cnt = 0;
}
}

#endif //TINYSTL_TEST_TEST_UTILS_H_
