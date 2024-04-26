#include <exception>
#include <memory>
#include <iostream>
#include <thread>

#include "EzNet/Socket/StreamSocket.hpp"
#include "EzNet/Socket/SocketExceptions.hpp"

namespace tab {

bool StreamSocket::connect(const Address& target, 
                           const time_t wait_time_milli, 
                           const size_t try_times) {
    // Check address family and protocol.
    if (target.getAF() != this->addr_.getAF()) {
        throw InvalidAddressException(
            "StreamSocket::connect(): ERROR: "
            "Target address family is not same as this socket's.");
    }

    auto addr = target.get();
    for (size_t i = 0; i < try_times; ++i) {
        if (_connect(this->get(), addr.get(), target.getSize()) != 0)
            std::this_thread::sleep_for (
                std::chrono::milliseconds(wait_time_milli));
        else
            return true;
    }
    return false;
}


int StreamSocket::send(const char* buf, int size, bool block) {
    if (buf != nullptr) {
        if (!block)
            if (!this->writable())
                return 0;
        return _send(this->get(), buf, size);
    }
    return -1;
}


int StreamSocket::recv(void* buf, int size, bool block) {
    if (buf != nullptr) {
        if (!block)
            if (!this->readable())
                return 0;
        return _recv(this->get(), (char*)buf, size, 0);
    }
    return -1;
}

std::string StreamSocket::recv(int buffer_size, bool block) {
    std::unique_ptr<char[]> buffer(new char[buffer_size]{});
    if (this->recv(buffer.get(), buffer_size, block) != -1)
        return std::move(std::string(buffer.get(), buffer_size));
    else
        return std::string();
}


void StreamSocket::stopSend(void) noexcept {
    if (this->writable())
        this->shutdown(_Base::SEND);
}

void StreamSocket::stopRecv(void) noexcept {
    if (this->readable())
        this->shutdown(_Base::RECV);
}

bool ServerSocket::listen(int backlog) {
    int&& res = _listen(this->get(), backlog);
    if (res == 0)
        return true;
    else
        return false;
}

StreamSocket ServerSocket::accept(void) {
    socklen_t size = sizeof(sockaddr_t);
    sockaddr_t tmp;
    socket_t socket_temp = _accept(this->get(), &tmp, &size);
#ifdef _LINUX
  #ifndef INVALID_SOCKET
  #define INVALID_SOCKET -1
  #endif
#endif
    if (socket_temp == INVALID_SOCKET)
        throw std::runtime_error(
            "tab::ServerSocket::accept(): Failed to accept a connection.");
    return StreamSocket(socket_temp, std::move(Address(tmp)));
}


} // namespace tab