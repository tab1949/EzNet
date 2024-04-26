cmake_minimum_required(VERSION 3.2)

project(Test)

set(ROOT ../../../..)
set(INCLUDE ${ROOT}/include/tab)

include_directories(${INCLUDE})

add_executable(main main.cpp)