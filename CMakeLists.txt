cmake_minimum_required(VERSION 3.13)
project(TinySTL)
set(CMAKE_CXX_STANDARD 14)

# 此处配置 + CmakeLists.txt.in 复制自 https://github.com/google/googletest/blob/master/googletest/README.md
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# 自定义编译 生成测试
# 所有的src中的cpp 都在 impl 下
aux_source_directory(./src/impl DIR_SRC_SRCS)
# 测试cpp
aux_source_directory(./test DIR_TEST_SRCS)
# 生成测试
add_executable(TinySTLTest ${DIR_TEST_SRCS} ${DIR_SRC_SRCS})

target_link_libraries(TinySTLTest gtest_main)

# 自定义编译 生成实验(一些C++知识的整理）
aux_source_directory(./experiment DIR_EXP_SRCS)
add_executable(exp ${DIR_EXP_SRCS})
