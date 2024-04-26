CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(EN_TARGET_NAME EzNet)

add_subdirectory(Socket)
add_subdirectory(HTTP)
add_subdirectory(Utility)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} CURRENT_SRC)

list(APPEND EN_SRC ${CURRENT_SRC})

# EN_SRC has been defined in subdirectories.
set(EN_SRC ${EN_SRC} PARENT_SCOPE)