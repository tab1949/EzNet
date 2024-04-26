cmake_minimum_required(VERSION 3.2)

project(test)

set(CMAKE_CXX_STANDARD 17)
set(ROOT_DIR ../../../..)

include_directories(${ROOT_DIR}/include/tab)

aux_source_directory( ${ROOT_DIR}/src TEST_SRC)
aux_source_directory( ${ROOT_DIR}/src/HTTP TEST_SRC)
aux_source_directory( ${ROOT_DIR}/src/Socket TEST_SRC)
aux_source_directory( ${ROOT_DIR}/src/Utility TEST_SRC)

find_package(OpenSSL)
message    ("+---------Notice---------+")
if (NOT OpenSSL_FOUND) 
    message("| OpenSSL library is not |")
    message("| found on this computer,|")
    message("| so that SecureSocket is|")
    message("| unavailable.           |")
else()
    set(CONF_OPENSSL "OpenSSL")
    include_directories(${OPENSSL_INCLUDE_DIR})
    link_libraries(${OPENSSL_LIBRARIES})
    if (WIN32)
        link_libraries(crypt32)
    endif ()
    message("| OpenSSL library is     |")
    message("| found on this computer.|")
    message("|                        |")
endif ()
message    ("+------------------------+")

if (WIN32)
    link_libraries(ws2_32 mswsock)
endif ()

configure_file(${ROOT_DIR}/include/tab/EzNet/Basic/configure.h.in ../${ROOT_DIR}/include/tab/EzNet/Basic/configure.h @ONLY)

add_executable(test-transfer test_transfer.cpp ${TEST_SRC})
# add_executable(test-logical test_logical.cpp ${TEST_SRC})
