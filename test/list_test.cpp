#include <gtest/gtest.h>
#include <list>
#include <string>
#include <random>

#include "../src/list.h"
#include "test_utils.h"

namespace TinySTL {
namespace Test {

template<typename T>
using stdL = std::list<T>;

template<typename T>
using tsL = TinySTL::list<T>;

TEST(ListTest, ListCtor) {
  stdL<int> l1(10, 8);
  tsL<int> l2(10, 8);
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));

  int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  stdL<int> l3(std::begin(arr), std::end(arr));
  tsL<int> l4(std::begin(arr), std::end(arr));
  EXPECT_TRUE(TinySTL::Test::container_equal(l3, l4));

  auto l5(l1);
  auto l6(l2);
  EXPECT_TRUE(TinySTL::Test::container_equal(l5, l6));

  auto l7 = l1;
  auto l8 = l2;
  EXPECT_TRUE(TinySTL::Test::container_equal(l7, l8));
}

TEST(ListTest, Cap) {
  tsL<std::string> l1;
  EXPECT_TRUE(l1.empty());
  EXPECT_EQ(l1.size(), 0);

  std::string arr[] = {"1", "2", "3"};
  tsL<std::string> l2(std::begin(arr), std::end(arr));
  EXPECT_TRUE(!l2.empty());
  EXPECT_EQ(l2.size(), 3);
}

TEST(ListTest, Acess) {
  std::string arr[] = {"1", "2", "3"};
  tsL<std::string> l(std::begin(arr), std::end(arr));
  EXPECT_EQ(l.back(), arr[2]);
  EXPECT_EQ(l.front(), arr[0]);

  l.front() = "front";
  l.back() = "back";
  EXPECT_EQ(l.back(), "back");
  EXPECT_EQ(l.front(), "front");
}

TEST(ListTest, PushPop) {
  stdL<int> l1;
  tsL<int> l2;
  for (auto i = 0; i != 10; ++i) {
    l1.push_front(i);
    l2.push_front(i);
  }
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));
  for (auto i = 0; i != 10; ++i) {
    l1.push_back(i);
    l2.push_back(i);
  }
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));
  for (auto i = 0; i != 5; ++i) {
    l1.pop_back();
    l2.pop_back();
  }
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));
  for (auto i = 0; i != 5; ++i) {
    l1.pop_front();
    l2.pop_front();
  }
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));
}

TEST(ListTest, Insert) {
  stdL<int> l1;
  tsL<int> l2;

  l1.insert(l1.end(), 10, -1);
  l2.insert(l2.end(), 10, -1);
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));

  auto it1 = l1.begin();
  auto it2 = l2.begin();
  for (auto i = l1.size() / 2 + 1; i != 0; --i) {
    ++it1;
    ++it2;
  }
  l1.insert(it1, 1);
  l2.insert(it2, 1);
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));

  int arr[] = {1, 2, 3, 4, 5};
  it1 = l1.end();
  it2 = l2.end();
  l1.insert(it1, std::begin(arr), std::end(arr));
  l2.insert(it2, std::begin(arr), std::end(arr));
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));
}

TEST(ListTest, Erase) {
  stdL<int> l1;
  tsL<int> l2;
  for (auto i = 0; i != 100; ++i) {
    l1.push_back(i);
    l2.push_back(i);
  }

  l1.erase(l1.begin());
  l1.erase(--l1.end());
  l2.erase(l2.begin());
  l2.erase(--l2.end());
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));

  l1.erase(l1.begin(), l1.end());
  l2.erase(l2.begin(), l2.end());
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));
}

TEST(ListTest, Sort) {
  std::random_device rd;
  stdL<int> l1;
  tsL<int> l2;
  for (auto i = 0; i != 10; ++i) {
    auto ret = rd() % 65536;
    l1.push_back(ret);
    l2.push_back(ret);
  }

  l1.sort();
  l2.sort();
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));

  l1.sort(std::greater<int>());
  l2.sort(std::greater<int>());
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));
}
TEST(ListTest, Reverse) {
  int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  stdL<int> l1(std::begin(arr), std::end(arr));
  tsL<int> l2(std::begin(arr), std::end(arr));

  l1.reverse();
  l2.reverse();
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));
}
TEST(ListTest, Merge) {
  int arr1[] = {0, 1, 3, 5, 9}, arr2[] = {2, 4, 6, 7, 8};
  stdL<double> first1(std::begin(arr1), std::end(arr1)), second1(std::begin(arr2), std::end(arr2));
  tsL<double> first2(std::begin(arr1), std::end(arr1)), second2(std::begin(arr2), std::end(arr2));

  first1.merge(second1);
  first2.merge(second2);
  EXPECT_TRUE(TinySTL::Test::container_equal(first1, first2));
}
TEST(ListTest, Remove) {
  int arr[] = {17, 89, 7, 14, 89, 0, 1, 4};
  stdL<int> l1(std::begin(arr), std::end(arr));
  tsL<int> l2(std::begin(arr), std::end(arr));

  l1.remove(89);
  l2.remove(89);
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));

  auto func = [](int n) { return n % 2 == 0; };
  l1.remove_if(func);
  l2.remove_if(func);
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));
}
TEST(ListTest, Splice) {
  stdL<int> l1(10, 0), l3(10, 1);
  tsL<int> l2(10, 0), l4(10, 1);

  l1.splice(l1.begin(), l3);
  l2.splice(l2.begin(), l4);
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));

  auto l5 = l1;
  auto l6 = l2;
  l1.splice(l1.end(), l5, l5.begin());
  l2.splice(l2.end(), l6, l6.begin());
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));

  auto it1 = l1.begin();
  auto it2 = l2.begin();
  for (auto i = 0; i != l1.size() / 2; ++i) {
    ++it1;
    ++it2;
  }
  l1.splice(it1, l5, l5.begin(), l5.end());
  l2.splice(it2, l6, l6.begin(), l6.end());
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l2));
}
TEST(ListTest, OperatorEq) {
  tsL<int> l1(10, 2), l2(10, 1), l3(10, 2);
  EXPECT_TRUE(TinySTL::Test::container_equal(l1, l3));
  EXPECT_FALSE(TinySTL::Test::container_equal(l1, l2));
}

}
}
