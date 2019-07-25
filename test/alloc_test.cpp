#include <gtest/gtest.h>

#include "../src/__alloc.h"

namespace TinySTL {
namespace Test {

TEST(Alloc, Simple) {
  for (int i = 1; i < 1024; ++i) {
    auto a = static_cast<char *>(TinySTL::__alloc::allocate(i));
    for (int j = 0; j < i; ++j) {
      *(a + j) = 'l';
    }
    TinySTL::__alloc::deallocate(a, i);
  }
  EXPECT_TRUE(true);
}

TEST(Alloc, Zero) {
  auto ptr = TinySTL::__alloc::allocate(0);
  EXPECT_EQ(ptr, nullptr);
  TinySTL::__alloc::deallocate(ptr, 0);
  EXPECT_TRUE(true);
}

}
}
