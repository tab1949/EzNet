cmake_minimum_required(VERSION 3.2)

project(test)

set(ROOT_DIR ../../..)
set(INCLUDE_DIR ${ROOT_DIR}/include/tab)
set(SRC_DIR ${ROOT_DIR}/src)
set(SRC ${SRC_DIR}/HTTP/HTTP_Cookie.cpp)

include_directories(${INCLUDE_DIR})

add_executable(test_cookie cookie.cpp ${SRC})
add_executable(test_cookies cookies.cpp ${SRC})