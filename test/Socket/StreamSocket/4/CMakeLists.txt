cmake_minimum_required(VERSION 3.2)

project(TEST_STREAMSOCKET_IPV4)

include_directories(../../../../include/tab)

set(ROOT_DIR ../../../..)
set(SRC ${ROOT_DIR}/src/Socket/StreamSocket.cpp ${ROOT_DIR}/src/Utility/Address.cpp ${ROOT_DIR}/src/constants.cpp)

add_executable(test main.cpp ${SRC})
add_executable(benchmark benchmark.cpp ${SRC})