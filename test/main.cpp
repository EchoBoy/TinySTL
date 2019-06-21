#include "./alloc_test.h"
#include "./vector_test.h"

int main(int, char **) {
  AllocTest::test_all_case();
  VectorTest::test_all_case();
  return 0;
}
