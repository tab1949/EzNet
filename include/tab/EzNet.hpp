#ifndef __EZNET_HPP__
#define __EZNET_HPP__

#include <exception>
#include <stdexcept>

#include "EzNet/Basic/platform.h"

#ifdef EN_OPENSSL
#include <openssl/ssl.h>
#endif

#include "EzNet/Utility.hpp"

#include "EzNet/Socket/Socket.hpp"
#include "EzNet/Socket/StreamSocket.hpp"
#include "EzNet/Socket/DatagramSocket.hpp"
#include "EzNet/Socket/SecureSocket.hpp"

#include "EzNet/HTTP.hpp"

namespace tab{

class SystemEnv {
public:
    ~SystemEnv(void) {
    #ifdef EN_OPENSSL
        OPENSSL_cleanup();
    #endif // EN_OPENSSL
        socket_cleanup();
    }

    static int GetError(void) {
    #ifdef _WINDOWS
        return WSAGetLastError();
    #else
        return errno;
    #endif
    }

private:
    static SystemEnv env;
    
    SystemEnv(void) {
        if (socket_init() != 0)
            throw std::runtime_error(
                "Error(s) have occurred while initializing the environment.");
    #ifdef EN_OPENSSL
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
    #endif // EN_OPENSSL
    }
};

} // namespace tab

#endif // __EZNET_HPP__