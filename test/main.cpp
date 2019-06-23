#include "./alloc_test.h"
#include "./vector_test.h"
#include "../src/construct.h"
#include <iostream>

void stop() {
  std::cout << "press any key to end!" << std::endl;
  char a = 0;
  std::cin >> a;
}

// static_case 操作符 和 普通类型转换的区别
//void my_test() {
//  char c = 'a';
//  int *p = (int *) &c;
//  int *q = static_cast<int *>(&c);
//}

int main(int, char **) {
  AllocTest::test_all_case();
  VectorTest::test_all_case();
//  my_test();
  stop();
  return 0;
}
