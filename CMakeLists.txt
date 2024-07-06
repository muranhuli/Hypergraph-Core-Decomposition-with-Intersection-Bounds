cmake_minimum_required(VERSION 3.28)
project(K_SCC_core)


set(CMAKE_CXX_STANDARD 17)
# 启用所有警告并设置编译优化级别为最高
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

include_directories(lib)

add_executable(K_SCC_core main.cpp)

add_executable(KH_Core khcore/khcore.cpp)

add_executable(Static khcore/static.cpp)

