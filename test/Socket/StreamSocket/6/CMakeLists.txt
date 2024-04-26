cmake_minimum_required(VERSION 3.2)

project(TEST_STREAMSOCKET_IPV6)

include_directories(../../../../include/tab)

aux_source_directory(../../../../src/Socket SRC)
aux_source_directory(../../../../src/Utility SRC)

add_executable(test main.cpp ${SRC})