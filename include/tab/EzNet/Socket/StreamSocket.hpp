#ifndef __STREAMSOCKET_HPP__
#define __STREAMSOCKET_HPP__

// Import standard libraries.
#include <string>

// Other dependencies.
#include "EzNet/Basic/net_func.h"
#include "EzNet/Basic/net_type.h"
#include "Socket.hpp"

namespace tab {

// disable warning C4267 for this class
#ifdef _MSVC
#  pragma warning(push)
#  pragma warning(disable: 4267)
#endif

/**
 * @brief A kind of socket which uses TCP. Supports IPv4 and IPv6.
 */
class StreamSocket : public SocketBase<SOCK_STREAM, IPPROTO_TCP> {
public:
    static const int DISCONNECTED = -1;
private:
    using _Base = SocketBase<SOCK_STREAM, IPPROTO_TCP>;

public:
    /**
     * @brief Construct a new StreamSocket object 
     *        using the designated IP version.
     */
    StreamSocket(af_t af) : 
        _Base(af) {}
    
    /**
     * @brief Construct a StreamSocket using IP address and port.
     * 
     * @param addr The IP address to use.
     * @param port The port to use.
     */
    StreamSocket(const std::string& addr, port_t port) : 
        _Base(std::move(Address::GetAddrByName(addr))) {
        addr_.setPort(port);
    }
    
    StreamSocket(const StreamSocket&) = delete;

    StreamSocket(StreamSocket&& rv) noexcept : 
        _Base(std::move(rv)) {
    }

    StreamSocket(const Address& addr) :
        _Base(addr) {}

    virtual ~StreamSocket() {
        
    }

    virtual StreamSocket& operator=(const StreamSocket&) = delete;

    virtual StreamSocket& operator=(StreamSocket&& val) noexcept {
        _Base::operator=(std::move(val));
        return *this;
    }

    virtual void setAddr(const std::string& str) {
        _Base::setAddr(str);
    }

    virtual void setAddr(const std::string& addr, port_t port) {
        this->addr_.setAddrByName(addr);
        this->addr_.setPort(port);
    }


    /**
     * @brief Connect to the target
     * 
     * @param target Address of the target host
     * @param wait_time_milli Time interval between two attempts, in ms
     * @param try_times Number of attempts
     * @return true : Connected successfully
     * @return false : Failed
     */
    virtual bool connect(const Address& target, 
                         const time_t wait_time_milli, 
                         const size_t try_times) override;


    /**
     * @brief Connect to the target
     * 
     * @param target Address of the target host
     * @return true : Successful
     * @return false : Failed
     */
    virtual bool connect(const Address& target) {
        return this->connect(target, 50, 2);
    }

    virtual int send(const char* buf, int size, bool block = true) override;
    
    virtual int send(const std::string& buf, bool block = true) override {
        return this->send(buf.c_str(), buf.size(), block);
    }

    virtual int recv(void* buf, int size, bool block = true) override;
    
    virtual std::string recv(int buffer_size = 1024, 
                             bool block = true) override;


    virtual void stopSend(void) noexcept;

    virtual void stopRecv(void) noexcept;

    virtual void stop(void) noexcept {
        stopSend();
        stopRecv();
    }

protected:
    StreamSocket(socket_t s, Address&& a) : _Base(s, std::move(a)) {}

    friend class ServerSocket;
    
}; // class StreamSocket

#ifdef _MSVC
#  pragma warning(pop)
#endif

class StreamSocket4 : public StreamSocket {
public:
    StreamSocket4(void) : StreamSocket(AF_INET) {}

}; // class StreamSocket4


class StreamSocket6 : public StreamSocket {
public:
    StreamSocket6(void) : StreamSocket(AF_INET6) {}

}; // class StreamSocket6


class ServerSocket : public StreamSocket {
private:
    using _Base = StreamSocket;
    using _This_Type = ServerSocket;

public:    
    /**
     * @brief Construct a new StreamSocket object
     *        using the designated IP version.
     */
    ServerSocket(af_t af = AF_INET) : 
        _Base(af) {
    }
    
    /**
     * @brief Construct a StreamSocket using IP address and port.
     * 
     * @param addr The IP address to use.
     * @param port The port to use.
     */
    ServerSocket(const std::string& addr, port_t port) : 
        _Base(addr, port) {
    }

    ServerSocket(const Address& addr) :
        _Base(addr) {}

    virtual bool listen(int backlog = SOMAXCONN);

    virtual StreamSocket accept(void);

}; // class ServerSocket


class ServerSocket4 : public ServerSocket {
public:
    ServerSocket4(void) : ServerSocket(AF_INET){}

}; // class ServerSocket4


class ServerSocket6 : public ServerSocket {
public:
    ServerSocket6(void) : ServerSocket(AF_INET6){}

}; // class ServerSocket6


using TCPSocket = ServerSocket;
using TCPSocket4 = ServerSocket4;
using TCPSocket6 = ServerSocket6;

} // namespace tab

#endif // __STREAMSOCKET_HPP__