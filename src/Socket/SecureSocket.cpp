#include "EzNet/Socket/SecureSocket.hpp"

#ifdef EN_OPENSSL

#include <stdexcept>
#include <string>
#include <functional>

namespace tab {

static std::function<void(SSL_CTX*)> SSL_CTX_deleter([](SSL_CTX* ptr){
    if (ptr)
        SSL_CTX_free(ptr);
});

static std::function<void(SSL*)> SSL_deleter([](SSL* ptr){
    if (ptr)
        SSL_free(ptr);
});


SslContext::SslContext() {
    const SSL_METHOD* method = TLS_method();
    SSL_CTX* pCtx = SSL_CTX_new(method);
    if (!pCtx)
        throw SslLibraryException(
            "tab::SslContext::SslContext(): "
            "Unable to create an new SSL_CTX object.");
    context_.reset(pCtx, SSL_CTX_deleter);
} // SslContext::SslContext(ProtocolVersion)


SslContext::SslContext(const SslContext& ctx) : context_(ctx.context_) { }


SslContext::SslContext(SslContext&& ctx) : 
    context_(std::move(ctx.context_)) { }


SecureSocket::SecureSocket(SecureSocket&& ss) :
    _Base(std::move(ss)),
    ssl_context_(std::move(ss.ssl_context_)),
    ssl_(std::move(ss.ssl_)) {

} // SecureSocket::SecureSocket(SecureSocket&&)

#ifdef _MSVC
#  pragma warning(push)
#  pragma warning(disable:4244)
#endif
SecureSocket::SecureSocket(const SslContext& ctx, af_t af) : 
    _Base(af),
    ssl_context_(ctx.context_),
    ssl_(SSL_new(ssl_context_.get()), SSL_deleter) {

    SSL_set_fd(ssl_.get(), this->get());
    // SSL_CTX_set_verify(ssl_context_.get(), SSL_VERIFY_PEER, nullptr);

} // SecureSocket::SecureSocket(const SslContext&)
#ifdef _MSVC
#  pragma warning(pop)
#endif

#ifdef _MSVC
#  pragma warning(push)
#  pragma warning(disable:4244)
#endif
SecureSocket::SecureSocket(SslContext&& ctx, af_t af) :
    _Base(af),
    ssl_context_(std::move(ctx.context_)),
    ssl_(SSL_new(ssl_context_.get()), SSL_deleter) {

    SSL_set_fd(ssl_.get(), this->get());

} // SecureSocket::SecureSocket(SslContext&&)
#ifdef _MSVC
#  pragma warning(pop)
#endif


#ifdef _MSVC
#  pragma warning(push)
#  pragma warning(disable: 4244)
#endif
bool SecureSocket::connect(const Address& target, 
                           const time_t wait_time_milli, 
                           const size_t try_times) {
    if (!StreamSocket::connect(target, wait_time_milli, try_times))
        return false;
    int ret_code = SSL_connect(ssl_.get());
    if (ret_code <= 0) {
        std::string err_info;
        auto err_code = ERR_get_error();
        while (err_code != 0) {
            err_info.append(ERR_error_string(err_code, nullptr));
            err_info.append("; ");
            err_code = ERR_get_error();
        }
        throw SslLibraryException(
            std::string("tab::SecureSocket::connect():(OpenSSL) ")
             + err_info);
    }
        

    return true;
} //SecureSocket::connect(const Address&, const time_t, const size_t)
#ifdef _MSVC
#  pragma warning(pop)
#endif


int SecureSocket::send(const char* buf, int size, bool block) {
    if (!block)
        if (!this->writable())
            return 0;
    return SSL_write(ssl_.get(), buf, size);
} // SecureSocket::send(const char*, int, bool)


int SecureSocket::recv(void* buf, int size, bool block) {
    if (!block)
        if (!this->readable())
            return 0;
    return SSL_read(ssl_.get(), buf, size);
} // SecureSocket::recv(void*, int, bool) 


std::string SecureSocket::recv(int buffer_size, bool block) {
    std::unique_ptr<char[]> buffer(new char[buffer_size]{});
    if (this->recv(buffer.get(), buffer_size, block) > 0)
        return std::string(buffer.get(), buffer_size);
    else
        return std::string();
} // SecureSocket::recv(int, bool)


} // namespace tab

#endif // EN_OPENSSL