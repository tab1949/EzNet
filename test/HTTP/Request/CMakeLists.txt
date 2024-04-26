cmake_minimum_required(VERSION 3.2)

project(Test)

set(ROOT_DIR ../../..)
set(SRC_DIR ${ROOT_DIR}/src)
set(INCLUDE_DIR ${ROOT_DIR}/include/tab)

include_directories(${INCLUDE_DIR})

set(SRC ${SRC_DIR}/HTTP/HTTP_Request.cpp ${SRC_DIR}/HTTP/HTTP_Header.cpp ${SRC_DIR}/HTTP/HTTP_Cookie.cpp)

add_executable(test-request basic_Request.cpp ${SRC})