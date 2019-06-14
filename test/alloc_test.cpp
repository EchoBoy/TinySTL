#include "./alloc_test.h"

namespace alloc_test {

void test_case1() {
  for (int i = 1; i < 1024; ++i) {
    auto a = (char *) TinySTL::alloc::allocate(i);
    for (int j = 0; j < i; ++j) {
      *(a + j) = 'l';
    }
    TinySTL::alloc::deallocate(a, i);
  }
}

void test_case2() {
  auto ptr = TinySTL::alloc::allocate(0);
  TinySTL::alloc::deallocate(ptr, 0);
}

void test_case3() {
  for (int i = 0; i < 1000; ++i) {
    TinySTL::alloc::allocate(i);
  }
}

void test_all_case() {
  test_case1();
  test_case2();
  test_case3();
}

}
