cmake_minimum_required(VERSION 3.2)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} CURRENT_SRC)

list(APPEND EN_SRC ${CURRENT_SRC})

set(EN_SRC ${EN_SRC} PARENT_SCOPE)
