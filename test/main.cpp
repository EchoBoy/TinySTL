#include "./alloc_test.h"
#include "./vector_test.h"
#include <iostream>

void stop() {
  std::cout << "press any key to end!" << std::endl;
  char a = 0;
  std::cin >> a;
}

int main(int, char **) {
  AllocTest::test_all_case();
  VectorTest::test_all_case();

  stop();
  return 0;
}
