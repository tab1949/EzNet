#ifndef __HTTP_CLIENT_HPP__
#define __HTTP_CLIENT_HPP__

#include <utility>

#include "EzNet/Socket/StreamSocket.hpp"
#include "EzNet/Socket/SecureSocket.hpp"
#include "EzNet/HTTP/HTTP_Session.hpp"
#include "EzNet/Utility/Network/URL.hpp"

namespace tab {

class HttpClient {
public:
    /**
     * @brief Construct a new Http Client object
     */
    HttpClient(void) {
        // addSocket(ip);
    }
    
    /**
     * @brief Construct a new HttpClient object(move)
     * 
     * @param client the xvalue
     */
    HttpClient(HttpClient&& client) noexcept {
        operator=(std::move(client));
    }
    
    ~HttpClient() {

    }
    
    /**
     * @brief Move a HttpClient object
     */
    HttpClient& operator=(HttpClient&&) noexcept {
        return *this;
    }


    /**
     * @brief Swap with the given HttpClient
     */
    HttpClient& swap(HttpClient&) noexcept {
        return *this;
    }


    /**
     * @brief Make a HttpSessionClient object to communicate with the server
     * 
     * @param url The target
     * @return HttpSessionClient
     */
    HttpSessionClient target(const URL& url);

protected:
    // EMPTY

}; // class HttpClient


} // namespace tab

#endif // __HTTP_CLIENT_HPP__
