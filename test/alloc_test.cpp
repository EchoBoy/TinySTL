#include "../src/alloc.h"
#include <gtest/gtest.h>

TEST(Alloc, Simple) {
  for (int i = 1; i < 1024; ++i) {
    auto a = static_cast<char *>(TinySTL::alloc::allocate(i));
    for (int j = 0; j < i; ++j) {
      *(a + j) = 'l';
    }
    TinySTL::alloc::deallocate(a, i);
  }
  EXPECT_TRUE(true);
}

TEST(Alloc, Zero) {
  auto ptr = TinySTL::alloc::allocate(0);
  EXPECT_EQ(ptr, nullptr);
  TinySTL::alloc::deallocate(ptr, 0);
  EXPECT_TRUE(true);
}

