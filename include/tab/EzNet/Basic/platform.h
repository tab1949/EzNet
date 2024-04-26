#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "configure.h"

#ifdef _MSC_VER
#  define _MSVC
#endif // _MSC_VER
#ifdef __GNUC__
#  define _GCC
#endif // __GNUC__

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32))
#  ifndef _WINDOWS
#    define _WINDOWS
#  endif 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#elif (defined(__linux__))
#  define _LINUX
#endif

#ifdef _WINDOWS
#  ifdef _GCC
#    ifdef _WIN32_WINNT
#    undef _WIN32_WINNT
#    define _WIN32_WINNT 0x0600
#    endif
#  endif
#  include <WinSock2.h>
#  include <WS2tcpip.h>
#  include <mswsock.h>
#  ifdef _MSVC
#    pragma comment(lib, "ws2_32.lib")
#  endif // _MSVC
#endif // _WINDOWS

#ifdef _LINUX
#  include <arpa/inet.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <unistd.h>
#  include <netdb.h>
#  include <fcntl.h>
#endif // _LINUX

#ifdef CONF_OPENSSL
#  define EN_OPENSSL 
#endif // CONF_OPENSSL

#endif // __PLATFORM_H__