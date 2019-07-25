#include <gtest/gtest.h>
#include <stack>
#include <string>

#include "../src/stack.h"

namespace TinySTL {
namespace Test {

template<typename T>
using stdSt = std::stack<T>;
template<typename T>
using tsSt = TinySTL::stack<T>;

TEST(StackTest, Ctor) {
  stdSt<int> st1;
  tsSt<int> st2;

  for (auto i = 0; i != 10; ++i) {
    st1.push(i);
    st2.push(i);
  }
  for (auto i = 0; i != 10; ++i) {
    EXPECT_EQ(st1.top(), st2.top());
    int a = st2.size();
    st1.pop();
    st2.pop();
  }
}
TEST(StackTest, Empty) {
  tsSt<std::string> st;
  EXPECT_TRUE(st.empty());
  EXPECT_EQ(st.size(), 0);

  st.push("one");
  st.push("two");
  EXPECT_FALSE(st.empty());
  EXPECT_EQ(st.size(), 2);
}
TEST(StackTest, Copy) {
  tsSt<int> st1;
  for (auto i = 0; i != 5; ++i)
    st1.push(i);
  auto st2(st1);
  EXPECT_TRUE(st1 == st2);
  EXPECT_FALSE(st1 != st2);
}
TEST(StackTest, Push) {
  tsSt<int> st1, st2;
  st1.push(1);
  st1.push(2);
  st1.push(3);
  st2.push(1);
  st2.push(2);
  EXPECT_TRUE(st1.size() == 3 && st2.size() == 2);
  st1.swap(st2);
  EXPECT_TRUE(st1.size() == 2 && st2.size() == 3);
  // TODO：这里应该调用stack内部定义的friend swap，而不是algorithm中的swap。
  TinySTL::swap(st1, st2);
  EXPECT_TRUE(st1.size() == 3 && st2.size() == 2);
}

}
}
