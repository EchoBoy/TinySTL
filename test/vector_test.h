#ifndef TINYSTL_TEST_VECTOR_TEST_H_
#define TINYSTL_TEST_VECTOR_TEST_H_

#include <vector>
#include <string>
#include <array>
#include "../src/vector.h"
#include "test_utils.h"

namespace VectorTest {

template<typename T>
using stdVec = std::vector<T>;

template<typename T>
using tsVec = TinySTL::vector<T>;

void test_all_case();

}

#endif //TINYSTL_TEST_VECTOR_TEST_H_
