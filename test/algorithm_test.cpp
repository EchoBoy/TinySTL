#include <gtest/gtest.h>
#include <algorithm>
#include <vector>

#include "../src/algorithm.h"
#include "test_utils.h"

namespace TinySTL {
namespace AlgorithmTEST {

TEST(HeapTEST, heap2) {
  std::vector<int> test_case{1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -1, -2, -3};
  std::vector<int> v1(test_case);
  std::vector<int> v2(test_case);
  std::make_heap(v1.begin(), v1.end());
  TinySTL::make_heap(v2.begin(), v2.end());
  while (!v1.empty() && !v2.empty()) {
    EXPECT_TRUE(v1.front() == v2.front());
    v1.pop_back();
    v2.pop_back();
  }
}

TEST(HeapTEST, heap) {
  int test_case[] = {10, 20, 30, 5, 15};
  std::vector<int> v1(test_case, test_case + 5);
  std::vector<int> v2(test_case, test_case + 5);

  // TODO:标准的 make_heap 最后一个元素（*(last-1)）没有算在 heap 中，而我这里算
  std::make_heap(v1.begin(), v1.end());
  TinySTL::make_heap(v2.begin(), v2.end());
  EXPECT_TRUE(std::is_heap(v1.begin(), v1.end()));
  EXPECT_TRUE(std::is_heap(v2.begin(), v2.end()));

  while (!v1.empty()) {
    std::pop_heap(v1.begin(), v1.end());
    TinySTL::pop_heap(v2.begin(), v2.end());
    EXPECT_EQ(v1.back(), v2.back());
    v1.pop_back();
    v2.pop_back();
  }

  v1 = std::vector<int>(test_case, test_case + 5);
  TinySTL::make_heap(v1.begin(), v1.end());
  v2 = v1;

  v1.push_back(99);
  std::push_heap(v1.begin(), v1.end());
  v2.push_back(99);
  TinySTL::push_heap(v2.begin(), v2.end());
  EXPECT_TRUE(std::is_heap(v1.begin(), v1.end()));
  EXPECT_TRUE(std::is_heap(v2.begin(), v2.end()));

  std::sort_heap(v1.begin(), v1.end());
  TinySTL::sort_heap(v2.begin(), v2.end());
  EXPECT_TRUE(TinySTL::Test::container_equal(v1, v2));
}

}
}
