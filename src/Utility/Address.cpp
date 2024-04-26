#include <functional>
#include <algorithm>
#include <cstring>

#include "EzNet/Utility/Network/Address.hpp"
#include "EzNet/Basic/net_func.h"

namespace tab {

Address& Address::operator=(const Address& val) {
    if (addr_.sa.sa_family != val.addr_.sa.sa_family)
        throw std::logic_error(
            "tab::Address::operator=(const Address&): "
            "Copying between two different address families is illegal.");
    addr_     = val.addr_;
    protocol_ = val.protocol_;
    return *this;
}

bool Address::operator==(const Address& val) const noexcept {
    return std::equal(addr_.data, addr_.data + sizeof(addr_), 
                      val.addr_.data, val.addr_.data + sizeof(addr_));
}

port_t Address::getnPort(void) const noexcept {
    return *((port_t*)addr_.sa.sa_data);
}

port_t Address::gethPort(void) const noexcept {
    return ntohs(getnPort());
}

int Address::getSize(void) const noexcept {
    if (addr_.sa.sa_family == AF_INET)
        return (int)sizeof(sockaddr_in_t);
    else
        return (int)sizeof(sockaddr_in6_t);
}

void Address::setAF(af_t af) {
    if (af != AF_INET && af != AF_INET6)
        throw InvalidAddressFamilyException(
            "Address::setAF(): The given address family is not supported.");
    addr_.sa.sa_family = af;
}

bool Address::setAddrByName(const std::string& hostname, 
                            bool allow_exception) {
    addrinfo* info = nullptr;
    getaddrinfo(hostname.c_str(), nullptr, nullptr, &info);
    if (info != nullptr) {
        if (info->ai_family == AF_INET && addr_.sa.sa_family == AF_INET) {
            addr_.sa = *info->ai_addr;
            // protocol_ = protocol_t(info->ai_protocol);
            return true;
        }
        else if (info->ai_family == AF_INET6 && 
                 addr_.sa.sa_family == AF_INET6) {
            addr_.sa6 = *(sockaddr_in6_t*)(info->ai_addr);
            // protocol_ = protocol_t(info->ai_protocol);
            return true;
        }
        else {
            // Only throw when 'allow_exception' is 'true'.
            if (allow_exception)
                throw InvalidAddressFamilyException(
                    "Address::setAddrByName(): The address family of "
                    "'hostname' is different from this address's.");
        }
    }
    else if (allow_exception) {
        throw std::runtime_error(
            "Address::setAddrByName(): Failed to call 'getaddrinfo()'.");
    }
    return false;
}

void Address::sethPort(port_t port) noexcept {
    setnPort(htons(port));
}

void Address::setnPort(port_t port) noexcept {
    *((port_t*)addr_.sa.sa_data) = port;
}

Address Address::GetAddrByName(const std::string& hostname) {
    addrinfo* info = nullptr;
    getaddrinfo(hostname.c_str(), nullptr, nullptr, &info);
    if (info != nullptr) {
        if (info->ai_family == AF_INET) {
            return Address(
                *info->ai_addr, 
                static_cast<protocol_t>(info->ai_protocol));
        }
        else if (info->ai_family == AF_INET6) {
            Address ret(AF_INET6);
            ret.addr_.sa6 = *(sockaddr_in6_t*)(info->ai_addr);
            ret.protocol_ = static_cast<protocol_t>(info->ai_protocol);
            return ret;
        }
    }
    throw std::runtime_error(
        "Address::GetAddrByName(): Unable to build an object of 'Address'.");
    // return Address();
}

Address Address::GetLocalAddress(void) {
    char name[128]{};
    if (gethostname(name, 128) != 0)
        throw std::runtime_error(
            "Address::GetLocalAddress(): Unable to get local hostname.");
    hostent* host = gethostbyname(name);
    if (host == nullptr)
        throw std::runtime_error(
            "Address::GetLocalAddress(): Unable to get local infomation.");
    Address ret(host->h_addrtype);
    if (host->h_addrtype == AF_INET6)
        ret.addr_.sa6.sin6_addr = *(in6_addr*)host->h_addr;
    else
        ret.addr_.sa4.sin_addr = *(in_addr*)host->h_addr;
    return ret;
}

std::string Address::getStr(void) {
    if (addr_.sa.sa_family == AF_INET) {
        char buf[16];
        return std::string(
            inet_ntop(AF_INET, &(addr_.sa4.sin_addr), buf, 16));
    }
    else if (addr_.sa.sa_family == AF_INET6) {
        char buf[40];
        return std::string(
            inet_ntop(AF_INET6, &(addr_.sa6.sin6_addr), buf, 40));
    }
    return std::string();
}

// In class Address4:


void Address4::sethAddr(const in_addr4_t& addr) noexcept {
    setnAddr(htonl(addr));
}

void Address4::setnAddr(const in_addr4_t& addr) noexcept {
    addr_.sa4.sin_addr.s_addr = addr;
}

void Address4::sethAddr(const in_addr4_t& addr, port_t port) noexcept {
    sethAddr(addr);
    sethPort(port);
}

void Address4::setnAddr(const in_addr4_t& addr, port_t port) noexcept {
    setnAddr(addr);
    setnPort(port);
}

void Address4::setAddr(const char* addr) {
    addr_.sa4.sin_addr.s_addr = inet_addr(addr);
}

void Address4::setAddr(const std::string& addr) {
    addr_.sa4.sin_addr.s_addr = inet_addr(addr.c_str());
}

void Address4::sethPort(port_t port) noexcept {
    setnPort(htons(port));
}

void Address4::setnPort(port_t port) noexcept {
    addr_.sa4.sin_port = port;
}

port_t Address4::gethPort(void) const noexcept {
    return ntohs(getnPort());
}

port_t Address4::getnPort(void) const noexcept {
    return addr_.sa4.sin_port;
}


in_addr4_t Address4::getnIPAddr(void) const noexcept {
    return addr_.sa4.sin_addr.s_addr;
}

in_addr4_t Address4::gethIPAddr(void) noexcept {
    return ntohl(getnIPAddr());
}


} // namespace tab