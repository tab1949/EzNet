#ifndef __NET_FUNC_H__
#define __NET_FUNC_H__

#ifdef __cplusplus
// Import C++ headers.
#include <climits>
#include <stdexcept>
#else
// Import C headers.
#include <limits.h>
#endif // __cplusplus

#include "net_type.h"
#include "platform.h"


#ifdef __cplusplus

inline int _bind(socket_t s, const sockaddr_t* name, int namelen) {
    return bind(s, name, namelen);
}

inline int _shutdown(socket_t s, int how) {
    return shutdown(s, how);
}

inline int _listen(socket_t s, int backlog) {
    return listen(s, backlog);
}

inline socket_t _accept(socket_t s, sockaddr_t* addr, socklen_t* addrlen) {
    return accept(s, addr, addrlen);
}

inline int _connect(socket_t s, const sockaddr_t* name, 
                    int namelen = sizeof(sockaddr_t)) {
    return connect(s, name, namelen);
}

inline int _send(socket_t s, const char* buf, int len, int flag = 0) {
    return send(s, buf, len, flag);
}

inline int _recv(socket_t s, char* buf, int len, int flags) {
    return recv(s, buf, len, flags);
}

inline int _select(int nfds, fd_set* readfds, fd_set* writefds, 
                   fd_set* exceptfds, struct timeval* timeout) {
    return select(nfds, readfds, writefds, exceptfds, timeout);
}
#endif // __cplusplus


#ifdef _WINDOWS

inline int _close(socket_t fd) {
    return closesocket(fd);
}

inline size_t read(socket_t fd, void* buf, size_t nbytes) {
#ifdef __cplusplus
    if (nbytes > INT_MAX) {
        throw std::logic_error("read(): ERROR size offered.");
        std::terminate();
    }
#endif // __cplusplus
    return recv(fd, (char*)buf, (int)nbytes, 0);
}

inline size_t write(socket_t fd, const void* buf, size_t nbytes) {
#ifdef __cplusplus
    if (nbytes > INT_MAX) {
        throw std::logic_error("write(): ERROR size offered.");
        std::terminate();
    }
#endif // __cplusplus
    return send(fd, (const char*)buf, (int)nbytes, 0);
}

inline int socket_cleanup(void) {
    return WSACleanup();
}

inline int socket_init(void) {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data);
}

#endif // _WINDOWS


#ifdef _LINUX

#define socket_init() 0
#define socket_cleanup() 0

inline int _close(socket_t fd){
    close(fd);
    return 0;
}
#endif // _LINUX


#endif // __NET_FUNC_HPP__