#include <gtest/gtest.h>
#include <queue>
#include <string>

#include "../src/queue.h"

namespace TinySTL {
namespace Test {

template<typename T>
using stdQ = std::queue<T>;
template<typename T>
using tsQ = TinySTL::queue<T>;

TEST(QueueTest, Ctor) {
  stdQ<int> q1;
  tsQ<int> q2;

  for (auto i = 0; i != 10; ++i) {
    q1.push(i);
    q2.push(i);
  }
  for (auto i = 0; i != 10; ++i) {
    EXPECT_TRUE(q1.front() == q2.front());
    q1.pop();
    q2.pop();
  }
}
TEST(QueueTest, Equal) {
  tsQ<int> q1;
  for (auto i = 0; i != 10; ++i)
    q1.push(i);
  auto q2(q1);
  EXPECT_TRUE(q1 == q2);
  EXPECT_FALSE(q1 != q2);
}
TEST(QueueTest, Empty) {
  tsQ<int> q;
  EXPECT_TRUE(q.empty());
  EXPECT_EQ(q.size(), 0);

  q.push(10);
  q.push(11);
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(q.size(), 2);
}
TEST(QueueTest, Push){
  tsQ<std::string> q;
  q.push("front");
  q.push("back");

  EXPECT_TRUE(q.front() == "front");
}
TEST(QueueTest, Swap) {
  tsQ<int> q1, q2;

  q1.push(1);
  q1.push(2);
  q1.push(3);
  q2.push(1);
  q2.push(2);

  EXPECT_TRUE(q1.size() == 3 && q2.size() == 2);
  q1.swap(q2);
  EXPECT_TRUE(q1.size() == 2 && q2.size() == 3);
  TinySTL::swap(q1, q2);
  EXPECT_TRUE(q1.size() == 3 && q2.size() == 2);
}

}
}
