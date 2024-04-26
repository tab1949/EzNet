cmake_minimum_required(VERSION 3.2)

project(TEST_ADDRESS)

set(ROOT_DIR ../../..)
set(INCLUDE_DIR ${ROOT_DIR}/include/tab)
set(SRC_DIR ${ROOT_DIR}/src/Utility)

set(SRC ${SRC_DIR}/Address.cpp)

include_directories(${INCLUDE_DIR})

add_executable(test main.cpp ${SRC})