cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 17)

project(transform_test)

set(ROOT_DIR ../../..)

include_directories(${ROOT_DIR}/include/tab)

add_executable(test_lower_upper_case lower_upper_case.cpp ${ROOT_DIR}/src/Utility/Transform.cpp)
add_executable(test_convert_hex_str_num convert_hex_str_num.cpp ${ROOT_DIR}/src/Utility/Transform.cpp)