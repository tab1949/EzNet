#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

// Import standard libraries.
#include <functional>
#include <stdexcept>
#include <utility>

// Other dependencies.
#include "EzNet/Basic/net_func.h"
#include "EzNet/Utility/Network/Address.hpp"
#include "SocketExceptions.hpp"

namespace tab {

/**
 * @brief The interface of all kinds of sockets.
 * 
 */
class Socket {
public:
    virtual ~Socket() {};

    /**
     * @brief Check whether the socket is active.
     */
    virtual bool active(void) noexcept {
        return writable();
    }

    virtual socket_t get(void) const noexcept = 0;

    virtual Address getAddr(void) const noexcept = 0;

    virtual port_t getnPort(void) const noexcept = 0;
    virtual port_t gethPort(void) const noexcept = 0;
    virtual port_t getPort (void) const noexcept {
        return gethPort();
    }

    virtual void setnPort(port_t port) noexcept = 0;
    virtual void sethPort(port_t port) noexcept = 0;
    virtual void setPort (port_t port) {
        sethPort(port);
    }

    virtual void setAddr(const Address& addr) = 0;
    virtual void setAddr(Address&& addr) = 0;
    
    virtual bool setOpt(int, int, const char *, int) = 0;

    virtual bool bind(void) = 0;
    virtual bool bind(const Address& addr) = 0;
    virtual bool bind(port_t port) = 0;
    virtual bool bind(const std::string& addr, port_t port) = 0;

    virtual bool connect(const Address& target, 
                         const time_t wait_time_milli = 200, 
                         const size_t try_times = 5) = 0;

    virtual int send(const char* buf, int size, bool block = true) = 0;
    virtual int send(const std::string& buf, bool block = true) = 0;

    virtual int recv(void* buf, int size, bool block = true) = 0;
    virtual std::string recv(int buffer_size = 1024, bool block = true) = 0;

    virtual bool readable(int wait_microsecond = 0) const = 0;
    virtual bool writable(int wait_microsecond = 0) const = 0;

    enum SHUTDOWN {
#ifdef _WINDOWS
        RECV = SD_RECEIVE,
        SEND = SD_SEND,
        BOTH = SD_BOTH
#else // _LINUX
        RECV = SHUT_RD,
        SEND = SHUT_WR,
        BOTH = SHUT_RDWR
#endif // _WINDOWS
    };
    
    virtual bool shutdown(SHUTDOWN how = BOTH) = 0;

    virtual bool close(void) noexcept = 0;

protected:
    Socket(){};

private:
    Socket(Socket&&) = delete;
    Socket(const Socket&) = delete;

}; // class Socket


/**
 * @brief SocketBase class, the basic socket which provides some 
 * @brief normal methods to use socket.
 * 
 * @note It's the base class of StreamSocket and DatagramSocket.
 */
template <int Type_, protocol_t Protocol_, bool Allow_Exception_ = false>
class SocketBase : public Socket {
protected:
    static const socket_t NULL_SOCKET = 0;

public:
    /**
     * @brief Construct a new SocketBase object, 
     *        using the information provided.
     * 
     * @note This constructor will create a new socket.
     */
    SocketBase(af_t af) : 
        socket_(socket(af, Type_, Protocol_)),
        addr_(af, Protocol_)  { }

    SocketBase(SocketBase&& rv) noexcept : 
        socket_(rv.socket_),
        addr_(std::move(rv.addr_)),
        destroy_(rv.destroy_),
        closer_(std::move(rv.closer_)) {
        rv.socket_  = NULL_SOCKET;
        rv.destroy_ = false;
    }

    SocketBase(const SocketBase&) = delete;

    SocketBase(const Address& addr) : 
        socket_(socket(addr.getAF(), Type_, Protocol_)),
        addr_(addr) {
    }

    SocketBase(Address&& addr) : 
        socket_(socket(addr.getAF(), Type_, Protocol_)),
        addr_(std::move(addr)) {
    }
    
    /**
     * @brief Destroy the SocketBase object. 
     */
    virtual ~SocketBase() override {
        if (socket_ != NULL_SOCKET && destroy_) {
            closer_(this->get());
        }
    }

    virtual SocketBase& operator=(const SocketBase&) = delete;

    virtual SocketBase& operator=(SocketBase&& s) {
        addr_ = std::move(s.addr_);
        socket_   = s.socket_;
        s.socket_ = NULL_SOCKET;
        destroy_   = s.destroy_;
        s.destroy_ = false;
        s.closer_ = std::move(s.closer_);
        return *this;
    }

    virtual bool operator==(const SocketBase&) = delete;

    /**
     * @brief Swap with a SocketBase object.
     */
    virtual void swap(SocketBase& s) noexcept {
        // Swap all the members.
        std::swap(socket_, s.socket_);
        std::swap(destroy_, s.destroy_);
        addr_.swap(s.addr_);
        closer_.swap(s.closer_);
    }

    virtual socket_t get(void) const noexcept override {
        return socket_;
    }
    
    virtual Address getAddr(void) const noexcept override {
        return addr_;
    }

    virtual port_t getnPort(void) const noexcept override {
        return addr_.getnPort();
    }

    virtual port_t gethPort(void) const noexcept override {
        return addr_.gethPort();
    }

    virtual void setnPort(port_t port) noexcept override {
        addr_.setnPort(port);
    }
    
    virtual void sethPort(port_t port) noexcept override {
        addr_.sethPort(port);
    }

    virtual void setAddr(const Address& addr) override {
        if (addr.getAF() != addr_.getAF()) {
            throw InvalidAddressException(
                "SocketBase::setAddr(): ERROR: Invalid address family.");
            return;
        }
        if (addr.getProtocol() != addr_.getProtocol()) {
            throw InvalidAddressException(
                "SocketBase::setAddr(): ERROR: Invalid protocol.");
            return;
        }
        addr_.reset(addr);
    }

    virtual void setAddr(Address&& addr) override {
        if (addr.getAF() != addr_.getAF()) {
            throw InvalidAddressException(
                "SocketBase::setAddr(): ERROR: Invalid address family.");
            return;
        }
        if (addr.getProtocol() != addr_.getProtocol()) {
            throw InvalidAddressException(
                "SocketBase::setAddr(): ERROR: Invalid protocol.");
            return;
        }
        addr_.reset(std::move(addr));
    }

    virtual bool setOpt(int level, int optname, 
                        const char *optval, int optlen) override {
        if (!setsockopt(this->get(), level, optname, optval, optlen))
            return true;
        else
            return false;
    }
    
    virtual bool bind(void) override {
        if (socket_ == NULL_SOCKET)
            return false;
        auto saddr = addr_.get();
        int res = _bind(get(), (const sockaddr*)saddr.get(), addr_.getSize());
        if (res != 0)
            return false;
            //throw std::runtime_error("SocketBase::bind(): Cannot bind.");
        else
            return true;
    }

    virtual bool bind(const Address& addr) override {
        addr_.reset(addr);
        return bind();
    }

    virtual bool bind(port_t port) override {
        addr_.setPort(port);
        return bind();
    }

    virtual bool bind(const std::string& addr, port_t port) override {
        addr_.reset(Address4(addr, port, addr_.getProtocol()));
        return bind();
    }

    virtual bool readable(int wait_microsecond = 0) const override {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(get(), &fds);
        timeval tv{0, wait_microsecond};
        int res = select(static_cast<int>(get()) + 1, 
                         &fds, nullptr, nullptr, &tv);
        if (res == 1)
            return true;
        else
            return false;
    }

    virtual bool writable(int wait_microsecond = 0) const override {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(get(), &fds);
        timeval tv{0, wait_microsecond};
        int res = select(static_cast<int>(get())+1, 
                         nullptr, &fds, nullptr, &tv);
        if (res == 1)
            return true;
        else
            return false;
    }

    virtual bool shutdown(SHUTDOWN how = BOTH) override {
        if (socket_ == NULL_SOCKET)
            return false;
        int res = _shutdown(get(), how);
        if (res == 0) {
            closer_ = closer_c;
            return true;
        }
        else
            return false;
    }

    virtual bool close(void) noexcept override {
        if (socket_ == NULL_SOCKET)
            return false;
        int res = _close(get());
        if (res == 0) {
            closer_ = closer_none;
            return true;
        }
        else
            return false;
    }

    socket_t detach() {
        socket_ = NULL_SOCKET;
        destroy_ = false;
        return socket_;
    }
    
protected:
    // This is for some special usage. (e.g., accept())
    SocketBase(socket_t s, Address&& a) : socket_(s), addr_(std::move(a)) { }

    // These classes can access the ctor above.
    friend class StreamSocket;
    friend class ServerSocket;

    socket_t socket_;
    Address  addr_;
    bool     destroy_ = true;
    
    static void closer_s_c(socket_t s) {
        _shutdown(s, SD_BOTH);
        _close(s);
    }
    static void closer_s(socket_t s) {
        _shutdown(s, SD_BOTH);
    }
    static void closer_c(socket_t s) {
        _close(s);
    }
    static void closer_none(socket_t) {}

    std::function<void(socket_t)> closer_ = closer_s_c;
    // std::shared_ptr<socket_t> socket_;
    
}; // class SocketBase

} // namespace tab

#endif // __SOCKET_HPP__