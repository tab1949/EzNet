#include <iostream>
#include <memory>

#include "EzNet/HTTP/HTTP_Client.hpp"

namespace tab{

HttpSessionClient GetNewSession() {
#ifdef EN_OPENSSL
    SslContext ssl_ctx;
    HttpSessionClient ret([ssl_ctx](af_t af, char ty){
        if (ty == 1) // SecureSocket
            return std::shared_ptr<Socket>(new SecureSocket(ssl_ctx, af));
        else  // Common StreamSocket
            return std::shared_ptr<Socket>(new StreamSocket(af));
    });
#else
    HttpSessionClient ret([](af_t af, char){
        return std::shared_ptr<Socket>(new StreamSocket(af));
    });
#endif
    return ret;
}

HttpSessionClient HttpClient::target(const URL& url) {
    return GetNewSession().setURL(url);
}

} // namespace tab