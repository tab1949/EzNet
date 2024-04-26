#ifndef __DATAGRAMSOCKET_HPP__
#define __DATAGRAMSOCKET_HPP__

// Importing standard libraries.
#include <string>

// Other dependencies.
#include "EzNet/Basic/net_func.h"
#include "EzNet/Basic/net_type.h"
#include "Socket.hpp"
#include "SocketExceptions.hpp"

namespace tab {


class DatagramSocket : public SocketBase<SOCK_DGRAM, IPPROTO_UDP> {
private:
    using _Base = SocketBase<SOCK_DGRAM, IPPROTO_UDP>;

public:
    DatagramSocket(af_t af = AF_INET) : 
        _Base(af), dest_(af, IPPROTO_UDP) { }

    DatagramSocket(const DatagramSocket&) = delete;

    DatagramSocket(DatagramSocket&& ds) noexcept : 
        _Base(std::move(ds)), dest_(std::move(ds.dest_)) { }
    
    DatagramSocket(port_t port, const std::string& addr) : 
        DatagramSocket() {
        this->addr_.setPort(port);
        this->addr_.setAddrByName(addr);
    }


    /**
     * @brief Send data to the specified address.
     * 
     * @param to Destination
     * @param dat Data to send
     * @return int The bytes sent. If some error occurred, it will be <= 0.
     */
    int sendTo(const Address& to, const std::string& dat);

    /**
     * @brief Send data to the specified address.
     * 
     * @param to Destination
     * @param buf Data buffer
     * @param size Data length
     * @return int The bytes sent. If some error occurred, it will be <= 0.
     */
    int sendTo(const Address& to, const void* buf, int size);

    int recvFrom(const Address& from, std::string& data, int limit = 1024);
    int recvFrom(const Address& from, void* buf, int size);

    std::string recvFrom(const Address& from, int buf = 1024);

    Address recv(void* buf, int size);
    Address recv(std::string& data, int limit = 1024);
    
    std::string recv(int limit = 1024);

    /**
     * @brief Set the destination. It's 
     * @brief necessary when you need to call 'send()'
     * @brief with out the parameter of 'const Address& to'
     * 
     * @param dest Destination
     * @return DatagramSocket& 
     */
    DatagramSocket& setDest(const Address& dest) {
        if (dest.getAF() == addr_.getAF())
            dest_.reset(dest);
        else
            throw InvalidAddressException(
                "tab::DatagramSocket:setDest(): "
                "The address family of 'dest' is not same as this socket's.");
        return *this;
    }
    /**
     * @brief Alternative to method 'sendTo'
     * 
     * @param dat Data buffer
     * @param len  Data length
     * @return int The bytes sent. If some error occurred, it will be <= 0.
     */
    int send(const char* dat, int len, bool) override {
        return sendTo(dest_, dat, len);
    }

    /**
     * @brief See 'send(const char*, int)'
     */
    int send(const std::string& dat, bool) override {
        return sendTo(dest_, dat);
    }

   /**
    * @brief Alternative to method 'recvFrom'.
    * 
    * @param dat Receive buffer
    * @param len Length of the buffer
    * @return int Count of the bytes received
    */
    int recv(void* dat, int len, bool) override {
        return recvFrom(dest_, dat, len);
    }

    /**
     * @brief For more information, see 'int recv(void*, int)'.
     * 
     * @param len Length of receive buffer.
     * @return std::string The data received.
     */
    std::string recv(int len, bool) override {
        return recvFrom(dest_, len);
    }

    bool connect(const Address&, const time_t, const size_t) override {
        throw InvalidMethodException(
            "tab::DatagramSocket: DO NOT use 'connect' on a DatagramSocket. "
            "You can use 'setDest' instead.");
        return false;
    }

protected:
    Address dest_;
    
}; // class DatagramSocket

class DatagramSocketv4 : public DatagramSocket {
public:
    DatagramSocketv4(void) : DatagramSocket(AF_INET){}
};

class DatagramSocketv6 : public DatagramSocket { 
public:
    DatagramSocketv6(void) : DatagramSocket(AF_INET6){}
};


} // namespace tab

#endif // __DATAGRAMSOCKET_HPP__