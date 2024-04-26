cmake_minimum_required(VERSION 3.2)

project(TEST_EVENT)
set(CMAKE_CXX_STANDARD 17)

set(ROOT_DIR ../../../..)
set(INCLUDE_DIR ${ROOT_DIR}/include/tab)

include_directories(${INCLUDE_DIR})

add_executable(test main.cpp ${SRC})