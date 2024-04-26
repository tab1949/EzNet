#ifndef __SECURESOCKET_HPP__
#define __SECURESOCKET_HPP__

#include "EzNet/Basic/platform.h"

#ifdef EN_OPENSSL

#include <memory>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "StreamSocket.hpp"

namespace tab {

class SslContext;
class SecureSocket;
class SecureServerSocket;


/**
 * @brief Object-oriented encapsulation of SSL_CTX
 */
class SslContext {
public:
    SslContext();

    SslContext(const SslContext& ctx);

    SslContext(SslContext&& ctx);

    ~SslContext() = default;

    void setVerify(int mode, SSL_verify_cb callback = nullptr) {
        SSL_CTX_set_verify(context_.get(), mode, callback);
    }

    void setVerifyLocation(const char* CAfile, const char* CApath) {
        SSL_CTX_load_verify_locations(context_.get(), CAfile, CApath);
    }

    void setVerifyLocation(const std::string& CAfile, 
                          const std::string& CApath) {
        setVerifyLocation(CAfile.c_str(), CApath.c_str());
    }

    void setPrivateKeyFile(const char* file) {
        if (SSL_CTX_use_certificate_file(context_.get(), 
                                        file, SSL_FILETYPE_PEM) <= 0) {
            throw std::runtime_error(
                std::string("tab::SslContext::setPrivateKeyFile():(OpenSSL) ")
                + ERR_error_string(ERR_get_error(), nullptr));
        }
    }

private:
    friend class SecureSocket;
    std::shared_ptr<SSL_CTX> context_;

}; // SslContext


// disable warning C4267 for this class
#ifdef _MSVC
#  pragma warning(push)
#  pragma warning(disable: 4267)
#endif

class SecureSocket : public StreamSocket {
private:
    using _Base = StreamSocket;

public:
    SecureSocket(const SecureSocket&) = delete;
    
    /**
     * @brief Take a SecureSocket object
     */
    SecureSocket(SecureSocket&& ss);

    /**
     * @brief Construct a new SecureSocket object, 
     *        use the given SslContext object
     * 
     * @param ctx SslContext
     * @param af Address Family (default = AF_INET)
     */
    SecureSocket(const SslContext& ctx, af_t af = AF_INET);

    /**
     * @brief Construct a new SecureSocket object, 
     *        use the given SslContext object (move)
     * 
     * @param ctx SslContext
     * @param af Address Family (default = AF_INET)
     */
    SecureSocket(SslContext&& ctx, af_t af = AF_INET);

    /**
     * @brief Construct a new SecureSocket object, 
     *        use the specified TLS version
     * 
     * @deprecated Not recommended to use this constructor, because it will
     *             allocate a new SSL_CTX object, and this may costs a lot.
     * 
     * @param ver TLS version
     */
    SecureSocket() : SecureSocket(std::move(SslContext())) { }

    /**
     * @brief Connect to the target
     * 
     * @param target Address of the target host
     * @param wait_time_milli Time interval between two attempts, in ms
     * @param try_times Number of attempts
     * @return true : Connected successfully
     * @return false : Failed
     */
    bool connect(const Address& target, const time_t wait_time_milli, 
                 const size_t try_times) override;

    /**
     * @brief Connect to the target
     * 
     * @param target Address of the target host
     * @return true : Successful
     * @return false : Failed
     */
    bool connect(const Address& target) override {
        return this->connect(target, 50, 2);
    }

    int send(const char* buf, int size, bool block = true) override;
    
    int send(const std::string& buf, bool block = true) override {
        return this->send(buf.c_str(), buf.size(), block);
    }

    int recv(void* buf, int size, bool block = true) override;
    
    std::string recv(int buffer_size = 1024, bool block = true) override;

protected:
    std::shared_ptr<SSL_CTX> ssl_context_;
    std::shared_ptr<SSL> ssl_;

}; // class SecureSocket

#ifdef _MSVC
#pragma warning(pop)
#endif


class SecureServerSocket : public SecureSocket, private ServerSocket {

}; // class SecureServerSocket


} // namespace tab

#endif // EN_OPENSSL

#endif //__SECURESOCKET_HPP__