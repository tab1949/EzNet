#include "EzNet/Socket/DatagramSocket.hpp"

namespace tab {

#ifdef _MSVC
#pragma warning(push)
#pragma warning(disable: 4267)
#endif
int DatagramSocket::sendTo(const Address& to, const std::string& dat) {
    return sendto(this->get(), 
        dat.c_str(), dat.size(), 0, to.get().get(), to.getSize());
}
#ifdef _MSVC
#pragma warning(pop)
#endif

int DatagramSocket::sendTo(const Address& to, const void* buf, int size) {
    return sendto(this->get(), 
        (const char*)buf, size, 0, to.get().get(), to.getSize());
}


int DatagramSocket::recvFrom(const Address& from, 
                            std::string& data, int limit) {
    socklen_t&& size = from.getSize();
    std::unique_ptr<char[]> buf (new char[limit]{});
    int res 
        = recvfrom(this->get(), buf.get(), limit, 0, from.get().get(), &size);
    data.assign(buf.get());
    return res;
}

int DatagramSocket::recvFrom(const Address& from, void* buf, int size) {
    socklen_t&& s = from.getSize();
    return recvfrom(this->get(), (char*)buf, size, 0, from.get().get(), &s);
}


std::string DatagramSocket::recvFrom(const Address& from, int buf) {
    socklen_t&& size = from.getSize();
    std::unique_ptr<char[]> tmp(new char[buf]{});
    recvfrom(this->get(), tmp.get(), buf, 0, from.get().get(), &size);
    return tmp.get();
}

Address DatagramSocket::recv(void* buf, int size) {
    sockaddr_t addr {};
    socklen_t len = sizeof(addr);
    recvfrom(this->get(), (char*)buf, size, 0, &addr, &len);
    return std::move(Address(addr, IPPROTO_UDP));
}

Address DatagramSocket::recv(std::string& data, int limit) {
    sockaddr_t addr {};
    socklen_t len = sizeof(addr);
    std::unique_ptr<char[]> buf(new char[limit]{});
    recvfrom(this->get(), buf.get(), limit, 0, &addr, &len);
    data.clear();
    data.assign(buf.get(), limit);
    return std::move(Address(addr, IPPROTO_UDP));
}

std::string DatagramSocket::recv(int limit) {
    std::unique_ptr<char[]> buf(new char[limit]{});
    recv((void*)buf.get(), limit);
    return std::move(std::string(buf.get(), limit));
}

} // namespace tab