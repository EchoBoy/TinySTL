#include <gtest/gtest.h>
#include <deque>
#include <string>

#include "../src/deque.h"
#include "test_utils.h"

namespace TinySTL {
namespace Test {

using TinySTL::Test::container_equal;

template<typename T>
using stdDQ = std::deque<T>;

template<typename T>
using tsDQ = TinySTL::deque<T>;

TEST(DequeTest, Ctor) {
  stdDQ<int> dq1(10, 8);
  tsDQ<int> dq2(10, 8);
  EXPECT_TRUE(TinySTL::Test::container_equal(dq1, dq2));

  int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  stdDQ<int> dq3(std::begin(arr), std::end(arr));
  tsDQ<int> dq4(std::begin(arr), std::end(arr));
  EXPECT_TRUE(TinySTL::Test::container_equal(dq3, dq4));

  auto dq5(dq1);
  auto dq6(dq2);
  EXPECT_TRUE(TinySTL::Test::container_equal(dq5, dq6));

  auto dq7 = dq3;
  auto dq8 = dq4;
  EXPECT_TRUE(TinySTL::Test::container_equal(dq7, dq8));

  auto dq9 = std::move(dq7);
  auto dq10 = std::move(dq8);
  EXPECT_TRUE(TinySTL::Test::container_equal(dq9, dq10));
}
TEST(DequeTest, Size) {
  tsDQ<int> dq1;
  EXPECT_TRUE(dq1.empty());
  EXPECT_EQ(dq1.size(), 0);

  tsDQ<std::string> dq2(10, "hello");
  EXPECT_FALSE(dq2.empty());
  EXPECT_EQ(dq2.size(), 10);
}
TEST(DequeTest, SetValue) {
  stdDQ<std::string> dq1(10, "10");
  tsDQ<std::string> dq2(10, "10");

  dq1[0] = "0";
  dq1[9] = "9";
  dq2[0] = "0";
  dq2[9] = "9";

  EXPECT_TRUE(dq1.front() == dq2.front());
  EXPECT_TRUE(dq1.back() == dq2.back());
}
TEST(DequeTest, OPs) {
  stdDQ<int> dq1;
  tsDQ<int> dq2;

  for (auto i = 0; i != 10; ++i) {
    dq1.push_back(i);
    dq2.push_back(i);
  }
  EXPECT_TRUE(TinySTL::Test::container_equal(dq1, dq2));

  for (auto i = 10; i != 20; ++i)
    dq1.push_front(i);
  for (auto i = 10; i != 20; ++i)
    dq2.push_front(i);

  EXPECT_TRUE(TinySTL::Test::container_equal(dq1, dq2));

  for (auto i = 0; i != 5; ++i) {
    dq1.pop_back();
    dq2.pop_back();
  }
  EXPECT_TRUE(TinySTL::Test::container_equal(dq1, dq2));

  for (auto i = 0; i != 5; ++i) {
    dq1.pop_front();
    dq2.pop_front();
  }
  EXPECT_TRUE(TinySTL::Test::container_equal(dq1, dq2));
}
TEST(DequeTest, Swap) {
  int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  tsDQ<int> foo(arr, arr + 3), bar(arr + 3, arr + 10);

  EXPECT_TRUE(foo.size() == 3 && bar.size() == 7);
  foo.swap(bar);
  EXPECT_TRUE(foo.size() == 7 && bar.size() == 3);
  TinySTL::swap(foo, bar);
  EXPECT_TRUE(foo.size() == 3 && bar.size() == 7);
}
TEST(DequeTest, Clear) {
  stdDQ<double> dq1(10);
  tsDQ<double> dq2(10);
  for (auto i = 0; i < 10; ++i) {
    dq1.push_back(i);
    dq2.push_back(i);
  }
  EXPECT_TRUE(TinySTL::Test::container_equal(dq1, dq2));

  dq1.clear();
  dq2.clear();
  EXPECT_TRUE(TinySTL::Test::container_equal(dq1, dq2));

  for (auto i = 0 ; i< 10; ++i) {
    dq1.push_front(i);
    dq2.push_front(i);
  }
  EXPECT_TRUE(TinySTL::Test::container_equal(dq1, dq2));
}

TEST(DequeTest, ZeroEle) {
  stdDQ<TinySTL::Test::EmptyStruct> dq1(10);
  tsDQ<TinySTL::Test::EmptyStruct> dq2(10);
  EXPECT_TRUE(TinySTL::Test::container_equal(dq1, dq2));
}

}
}
