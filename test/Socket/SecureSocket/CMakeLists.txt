cmake_minimum_required(VERSION 3.2)

project(TEST_SECURESOCKET)

include_directories(../../../include/tab)
include_directories(./)

aux_source_directory(../../../src SRC)
aux_source_directory(../../../src/Socket SRC)
aux_source_directory(../../../src/Utility SRC)

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
    link_libraries(${OPENSSL_LIBRARIES} crypt32)
    message("| OpenSSL library is     |")
    message("| found on this computer.|")
    message("|                        |")
endif ()
message    ("+------------------------+")

configure_file(../../../include/tab/EzNet/Basic/configure.h.in ../../../../include/tab/EzNet/Basic/configure.h @ONLY)


add_executable(test main.cpp ${SRC})