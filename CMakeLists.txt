cmake_minimum_required(VERSION 3.13)
project(TinySTL)

set(CMAKE_CXX_STANDARD 17)

add_executable(TinySTL src/alloc.h src/impl/alloc.cpp src/construct.h test/alloc_test.cpp test/alloc_test.h test/main.cpp src/type_traits.h src/uninitialized.h src/iterator.h src/vector.h src/allocator.h test/vector_test.cpp test/vector_test.h test/test_utils.h src/algorithm.h)
