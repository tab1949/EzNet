# Lowest CMake version requirement.
cmake_minimum_required(VERSION 3.2)

project(EzNet CXX C)

set(CMAKE_CXX_STANDARD 17)
set(EN_INCLUDE include/tab)

add_subdirectory(src)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

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
    message("| found on this computer,|")
    message("| SSL support is enabled.|")
endif ()
message    ("+------------------------+")

if (WIN32)
    link_libraries(ws2_32 mswsock)
endif ()

configure_file(./include/tab/EzNet/Basic/configure.h.in ./include/tab/EzNet/Basic/configure.h @ONLY)

add_library(EzNet STATIC ${EN_SRC})

target_include_directories(EzNet PUBLIC ${EN_INCLUDE})

install(
    TARGETS EzNet 
    EXPORT EzNet
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(
    FILES 
    ${EN_INCLUDE}/EzNet.hpp 
    DESTINATION include
)
install(
    FILES 
    ${EN_INCLUDE}/EzNet/HTTP.hpp 
    ${EN_INCLUDE}/EzNet/Utility.hpp 
    DESTINATION include/EzNet
)
install(
    FILES 
    ${EN_INCLUDE}/EzNet/Basic/configure.h 
    ${EN_INCLUDE}/EzNet/Basic/net_func.h
    ${EN_INCLUDE}/EzNet/Basic/net_type.h
    ${EN_INCLUDE}/EzNet/Basic/platform.h
    DESTINATION include/EzNet/Basic
)
install(
    FILES
    ${EN_INCLUDE}/EzNet/HTTP/HTTP_Client.hpp
    ${EN_INCLUDE}/EzNet/HTTP/HTTP_Cookie.hpp
    ${EN_INCLUDE}/EzNet/HTTP/HTTP_Header.hpp
    ${EN_INCLUDE}/EzNet/HTTP/HTTP_Protocol.hpp
    ${EN_INCLUDE}/EzNet/HTTP/HTTP_Request.hpp
    ${EN_INCLUDE}/EzNet/HTTP/HTTP_RequestLine.hpp
    ${EN_INCLUDE}/EzNet/HTTP/HTTP_Response.hpp
    ${EN_INCLUDE}/EzNet/HTTP/HTTP_Server.hpp
    ${EN_INCLUDE}/EzNet/HTTP/HTTP_Session.hpp
    ${EN_INCLUDE}/EzNet/HTTP/HTTP_StatusLine.hpp
    DESTINATION include/EzNet/HTTP
)
install(
    FILES
    ${EN_INCLUDE}/EzNet/Socket/DatagramSocket.hpp
    ${EN_INCLUDE}/EzNet/Socket/SecureSocket.hpp
    ${EN_INCLUDE}/EzNet/Socket/Socket.hpp
    ${EN_INCLUDE}/EzNet/Socket/SocketExceptions.hpp
    ${EN_INCLUDE}/EzNet/Socket/StreamSocket.hpp
    ${EN_INCLUDE}/EzNet/Socket/TcpServer.hpp
    ${EN_INCLUDE}/EzNet/Socket/TcpServerEvents.hpp
    DESTINATION include/EzNet/Socket
)
install(
    FILES
    ${EN_INCLUDE}/EzNet/Utility/Event/Event.hpp
    ${EN_INCLUDE}/EzNet/Utility/Event/EventMatcher.hpp
    DESTINATION include/EzNet/Utility/Event
)
install(
    FILES
    ${EN_INCLUDE}/EzNet/Utility/General/Exceptions.hpp
    ${EN_INCLUDE}/EzNet/Utility/General/Transform.hpp
    DESTINATION include/EzNet/Utility/General
)
install(
    FILES
    ${EN_INCLUDE}/EzNet/Utility/IO/IO.hpp
    DESTINATION include/EzNet/Utility/IO
)
install(
    FILES
    ${EN_INCLUDE}/EzNet/Utility/Memory/Memory.hpp
    DESTINATION include/EzNet/Utility/Memory
)
install(
    FILES
    ${EN_INCLUDE}/EzNet/Utility/Network/Address.hpp
    ${EN_INCLUDE}/EzNet/Utility/Network/URL.hpp
    DESTINATION include/EzNet/Utility/Network
)