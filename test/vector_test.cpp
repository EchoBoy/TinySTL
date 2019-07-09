#include <vector>
#include <string>
#include <array>
#include <gtest/gtest.h>

#include "../src/vector.h"
#include "test_utils.h"
namespace TinySTL {
namespace VectorTest {

using TinySTL::Test::container_equal;

template<typename T>
using stdVec = std::vector<T>;

template<typename T>
using tsVec = TinySTL::vector<T>;

TEST(VectorTest, Ctor) {
  stdVec<std::string> v1(10, "ligand");
  tsVec<std::string> v2(10, "ligand");
  EXPECT_TRUE(container_equal(v1, v2));

  stdVec<std::string> v3(10);
  tsVec<std::string> v4(10);
  EXPECT_TRUE(container_equal(v3, v4));

  std::array<std::string, 3> arr = {"abc", "def", "ghi"};
  stdVec<std::string> v5(std::begin(arr), std::end(arr));
  tsVec<std::string> v6(std::begin(arr), std::end(arr));
  EXPECT_TRUE(container_equal(v5, v6));
}

TEST(VectorTest, CCtorAndAssgin) {
  stdVec<int> temp1(10, 0);
  tsVec<int> temp2(10, 0);

  auto v1(temp1);
  auto v2(temp2);
  EXPECT_TRUE(container_equal(v1, v2));

  auto v5 = temp1;
  auto v6 = temp2;
  EXPECT_TRUE(container_equal(v5, v6));
}

TEST(VectorTest, PushBack) {
  tsVec<int> v1, v2;
  for (int i = 0; i != 100; ++i) {
    v1.push_back(i);
    v2.push_back(i);
  }
  EXPECT_TRUE(v1 == v2);
  EXPECT_FALSE(v1 != v2);
}

TEST(VectorTest, Iterator) {
  tsVec<int> my_vector;
  for (int i = 1; i <= 5; i++)
    my_vector.push_back(i);
  auto i = 1;
  for (auto it = my_vector.begin(); it != my_vector.end(); ++it, ++i) {
    EXPECT_EQ(*it, i);
  }
  i = 1;
  for (auto it = my_vector.cbegin(); it != my_vector.cend(); ++it, ++i) {
    EXPECT_EQ(*it, i);
  }
}

TEST(VectorTest, Size) {
  tsVec<int> v(11, 0);
  EXPECT_EQ(v.size(), 11);
  v.resize(5);
  EXPECT_EQ(v.size(), 5);
  v.resize(20);
  EXPECT_EQ(v.size(), 20);

  tsVec<int> v1;
  v.reserve(20);
  EXPECT_GE(v.capacity(), 20);
}

TEST(VectorTest, SetValue) {
  stdVec<int> v1(10);
  tsVec<int> v2(10);
  for (unsigned i = 0; i < 10; i++) {
    v1[i] = i;
    v2[i] = i;
  }
  EXPECT_TRUE(TinySTL::Test::container_equal(v1, v2));
  v1.front() = 99;
  v2.front() = 99;
  v1.back() = 100;
  v2.back() = 100;
  EXPECT_TRUE(TinySTL::Test::container_equal(v1, v2));

  stdVec<int> v3(5);
  tsVec<int> v4(5);

  auto p1 = v3.data();
  auto p2 = v4.data();
  *p1 = 10;
  ++p1;
  *p1 = 20;
  p1[2] = 100;

  *p2 = 10;
  ++p2;
  *p2 = 20;
  p2[2] = 100;
  EXPECT_TRUE(TinySTL::Test::container_equal(v3, v4));
}

TEST(VectorTest, Swap) {
  tsVec<int> foo(3, 100);
  tsVec<int> bar(2, 200);
  EXPECT_TRUE(TinySTL::Test::container_equal(foo, stdVec<int>{100, 100, 100}));
  EXPECT_TRUE(TinySTL::Test::container_equal(bar, stdVec<int>{200, 200}));

  foo.swap(bar);
  EXPECT_TRUE(TinySTL::Test::container_equal(bar, stdVec<int>{100, 100, 100}));
  EXPECT_TRUE(TinySTL::Test::container_equal(foo, stdVec<int>{200, 200}));
}
TEST(VectorTest, String) {
  stdVec<std::string> v1;
  tsVec<std::string> v2;
  v1.push_back("hello ");
  v1.push_back("world");
  v2.push_back("hello ");
  v2.push_back("world");
  EXPECT_TRUE(TinySTL::Test::container_equal(v1, v2));

  v1.pop_back();
  v2.pop_back();
  EXPECT_TRUE(TinySTL::Test::container_equal(v1, v2));
}

TEST(VectorTest, Insert) {
  stdVec<int> v1;
  tsVec<int> v2;

  v1.insert(v1.begin(), 0);
  v2.insert(v2.begin(), 0);
  v1.insert(v1.end(), 1);
  v2.insert(v2.end(), 1);
  EXPECT_TRUE(TinySTL::Test::container_equal(v1, v2));

  v1.insert(v1.begin() + v1.size() / 2, 10, 0);
  v2.insert(v2.begin() + v2.size() / 2, 10, 0);
  EXPECT_TRUE(TinySTL::Test::container_equal(v1, v2));

  int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  v1.insert(v1.end(), std::begin(arr), std::end(arr));
  v2.insert(v2.end(), std::begin(arr), std::end(arr));
  EXPECT_TRUE(TinySTL::Test::container_equal(v1, v2));
}

TEST(VectorTest, Erase) {
  stdVec<int> v1;
  tsVec<int> v2;
  for (int i = 1; i <= 10; i++) {
    v1.push_back(i);
    v2.push_back(i);
  }
  v1.erase(v1.begin() + 5);
  v2.erase(v2.begin() + 5);
  EXPECT_TRUE(TinySTL::Test::container_equal(v1, v2));

  v1.erase(v1.begin(), v1.begin() + 3);
  v2.erase(v2.begin(), v2.begin() + 3);
  EXPECT_TRUE(TinySTL::Test::container_equal(v1, v2));
}

namespace {
typedef TinySTL::Test::CountLife TestItem;

// 以上测试都是常规操作，这个考察构造，复制构造，析构和assignment operate。
TEST(VectorTest, ProInsert) {
  auto obj = TestItem();
  stdVec<TestItem> data(10);

  TestItem::set_zero_all();
  tsVec<TestItem> t;
  t.push_back(obj);
  t.insert(t.begin(), 3, obj);
  t.insert(t.begin(), obj);
  auto t_res = TestItem::ctorsubdtor();

  TestItem::set_zero_all();
  stdVec<TestItem> t1;
  t1.push_back(obj);
  t1.insert(t1.begin(), 3, obj);
  t1.insert(t1.begin(), obj);
  auto t1_res = TestItem::ctorsubdtor();
  EXPECT_EQ(t_res, t1_res);

  TestItem::set_zero_all();
  tsVec<TestItem> t3;
  t3.reserve(30);
  t3.insert(t3.begin(), 3, obj);
  t3.insert(t3.begin() + 2, data.begin(), data.end());
  t3.insert(t3.begin() + 2, data.begin(), data.end());
  auto t3_res = TestItem::ctorsubdtor();

  TestItem::set_zero_all();
  stdVec<TestItem> t4;
  t4.reserve(30);
  t4.insert(t4.begin(), 3, obj);
  t4.insert(t4.begin() + 2, data.begin(), data.end());
  t4.insert(t4.begin() + 2, data.begin(), data.end());
  auto t4_res = TestItem::ctorsubdtor();
  EXPECT_EQ(t3_res, t4_res);
}
TEST(VectorTest, ProErase) {
  TestItem::set_zero_all();
  stdVec<TestItem> v1(10);
  v1.erase(v1.begin());
  auto v1_res = TestItem::ctorsubdtor();

  TestItem::set_zero_all();
  tsVec<TestItem> v2(10);
  v2.erase(v2.begin());
  auto v2_res = TestItem::ctorsubdtor();
  EXPECT_EQ(v1_res, v2_res);

  TestItem::set_zero_all();
  v1.erase(v1.begin() + 1, v1.begin() + 5);
  v1_res = TestItem::ctorsubdtor();

  TestItem::set_zero_all();
  v2.erase(v2.begin() + 1, v2.begin() + 5);
  v2_res = TestItem::ctorsubdtor();
  EXPECT_EQ(v1_res, v2_res);
}
}
}
}
