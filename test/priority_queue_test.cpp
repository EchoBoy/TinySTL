#include <gtest/gtest.h>
#include <queue>
#include <vector>
#include <string>

#include "../src/priority_queue.h"

namespace TinySTL {
namespace Test {
template<class T>

using stdPQ = std::priority_queue<T>;
template<class T>
using tsPQ = TinySTL::priority_queue<T>;

TEST(PriorityQueueTest, Pop) {
  int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -1, -2, -3};
  stdPQ<int> pq1(std::begin(arr), std::end(arr));
  TinySTL::priority_queue<int, std::vector<int>> pq2(std::begin(arr), std::end(arr));

  while (!pq1.empty() && !pq2.empty()) {
    EXPECT_TRUE(pq1.top() == pq2.top());
    pq1.pop();
    pq2.pop();
  }
  EXPECT_TRUE(pq1.empty() && pq2.empty());
}
TEST(PriorityQueueTest, Empty) {
  tsPQ<std::string> pq;
  EXPECT_TRUE(pq.empty());

  pq.push("zxh");
  EXPECT_FALSE(pq.empty());
}
TEST(PriorityQueueTest, PushPop) {
  tsPQ<int> pq;
  auto i = 1;
  for (; i != 10; ++i) {
    pq.push(i);
    EXPECT_TRUE(pq.size() == i);
  }
  for (i = pq.size(); i != 0; --i) {
    pq.pop();
    EXPECT_TRUE(pq.size() == (i - 1));
  }
}
TEST(PriorityQueueTest, PushPop2) {
  stdPQ<int> pq1;
  tsPQ<int> pq2;

  pq1.push(30);
  pq1.push(100);
  pq1.push(25);
  pq1.push(40);

  pq2.push(30);
  pq2.push(100);
  pq2.push(25);
  pq2.push(40);

  while (!pq1.empty() && !pq2.empty()) {
    EXPECT_TRUE(pq1.top() == pq2.top());
    pq1.pop();
    pq2.pop();
  }
}
TEST(PriorityQueueTest, Swap) {
  tsPQ<int> foo, bar;
  foo.push(15);
  foo.push(30);
  foo.push(10);
  bar.push(101);
  bar.push(202);

  EXPECT_TRUE(foo.size() == 3 && bar.size() == 2);
  TinySTL::swap(foo, bar);
  EXPECT_TRUE(foo.size() == 2 && bar.size() == 3);
}
}
}
