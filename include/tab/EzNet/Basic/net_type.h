#ifndef __NET_TYPE_H__
#define __NET_TYPE_H__

#include "platform.h"

typedef unsigned short         address_family_t;
typedef unsigned long          in_addr4_t;
typedef address_family_t       af_t;
typedef unsigned short         port_t;
#if defined(_MSVC)
typedef IPPROTO                protocol_t;
#elif defined(_GCC)
typedef int                    protocol_t;
#endif
typedef struct sockaddr        sockaddr_t;
typedef sockaddr_t*            sockaddr_p_t;
typedef struct sockaddr_in     sockaddr_in_t;
typedef struct sockaddr_in6    sockaddr_in6_t;
typedef int                    socket_type_t;
typedef socket_type_t          st_t;

#ifndef SOCKET_ERROR
#  define SOCKET_ERROR (-1)
#endif

/// Defining types on different platforms.

#ifdef _WINDOWS

typedef SOCKET    socket_t;
typedef int       socklen_t;

#endif // _WINDOWS


#ifdef _LINUX

typedef int socket_t;

#endif // _LINUX

#endif // __NET_TYPE_H__