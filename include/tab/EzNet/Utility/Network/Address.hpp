#ifndef __ADDRESS_HPP__
#define __ADDRESS_HPP__

#include <string>
#include <memory>
#include <exception>
#include <stdexcept>
#include <utility>

#include "EzNet/Basic/net_type.h"

namespace tab{

class InvalidAddressFamilyException : public std::logic_error {
public:
    InvalidAddressFamilyException(const char* str) : std::logic_error(str) {}

}; // class InvalidAddressFamilyException

class Address {
public:
    static const Address LOCAL_ADDRESS_V4;
    static const Address LOCAL_ADDRESS_V6;

public:
    Address(const af_t& af = AF_INET,
            const protocol_t& p = static_cast<protocol_t>(IPPROTO_IP)) {
        if (af == AF_INET)
            addr_.sa4.sin_addr = LOCAL_ADDRESS_V4.addr_.sa4.sin_addr;
        else if (af == AF_INET6)
            addr_.sa6.sin6_addr = LOCAL_ADDRESS_V6.addr_.sa6.sin6_addr;
        else
            throw InvalidAddressFamilyException(
                "Address::Address(): "
                "The given address family is not supported.");
        addr_.sa.sa_family = af;
        protocol_          = p;
    }

    Address(const sockaddr_t& addr, 
            const protocol_t& p = static_cast<protocol_t>(IPPROTO_IP)) {
        addr_.sa  = addr;
        protocol_ = p;
    }

    Address(const std::string& str) : 
        Address(std::move(GetAddrByName(str))) { }

    Address(const Address& val) : 
        protocol_(val.protocol_), addr_(val.addr_) { }

    virtual ~Address() {}

    virtual void swap(Address& a) noexcept {
        std::swap(addr_, a.addr_);
        std::swap(protocol_, a.protocol_);
    }

    Address& operator=(const Address& val);

    bool operator==(const Address& val) const noexcept;

    bool operator!=(const Address& val) const noexcept {
        return !operator==(val);
    }

    virtual void reset(const Address& val) {
        operator=(val);
    }

    address_family_t getAF(void) const noexcept {
        return addr_.sa.sa_family;
    }

    protocol_t getProtocol(void) const noexcept {
        return protocol_;
    }

    /**
     * @brief Get the port. (In network byte order)
     */
    port_t getnPort(void) const noexcept;


    /**
     * @brief Get the port. (In host byte order)
     */
    port_t gethPort(void) const noexcept;

    /**
     * @brief Get the port. (In host byte order)
     */
    port_t getPort(void) const noexcept {
        return gethPort();
    }

    std::shared_ptr<sockaddr_t> get(void) const noexcept {
        sockaddr_t* ret \
            = (sockaddr_p_t) ( getAF() == AF_INET6 ? \
                (sockaddr_p_t)new sockaddr_in6_t(addr_.sa6) : 
                (sockaddr_p_t)new sockaddr_in_t(addr_.sa4));
        return std::shared_ptr<sockaddr_t>(ret);
    }

    int getSize(void) const noexcept;

    void setAF(af_t af);

    /**
     * @brief Set this object as the address of 'hostname'.
     * 
     * @param hostname The string of the address to set.
     * @param allow_exception Allow to throw an exception or not.
     *                        It's true by default.
     * @return If 'allow_exception' is set as 'false', 
     *         the return value will tell you 
     *         whether this operation is successful.
     */
    bool setAddrByName(const std::string& hostname, 
                       bool allow_exception = true);

    void setnPort(port_t port) noexcept;

    void sethPort(port_t port) noexcept;

    Address& setPort(port_t port) noexcept {
        sethPort(port);
        return *this;
    }

    Address& setProtocol(protocol_t proto) noexcept {
        protocol_ = proto;
        return *this;
    }

    static Address GetAddrByName(const std::string& hostname);

    static Address GetLocalAddress(void);

    std::string getStr(void);

protected:
    protocol_t protocol_;
    union {
        sockaddr_t     sa;
        sockaddr_in_t  sa4;
        sockaddr_in6_t sa6;
        char data[28];
    } addr_;

}; // class Address


class Address4 : public Address {
public:
    Address4(void) : Address(AF_INET, IPPROTO_TCP) { }

    Address4(const sockaddr_in_t& addr) : Address4() {
        addr_.sa4 = addr;
    }

    Address4(const char* addr, const port_t& port = 0, 
             const protocol_t& pro = IPPROTO_TCP) : Address(AF_INET, pro) {
        addr_.sa4.sin_addr.s_addr = inet_addr(addr);
        addr_.sa4.sin_port        = htons(port);
        protocol_ = pro;
    }

    Address4(in_addr4_t addr, const port_t& port = 0, 
             const protocol_t& pro = IPPROTO_TCP) : Address(AF_INET, pro) {
        addr_.sa4.sin_addr.s_addr = addr;
        addr_.sa4.sin_port        = htons(port);
        protocol_ = pro;
    }

    explicit Address4(const std::string& addr, const port_t& port = 0, 
             const protocol_t& pro = IPPROTO_TCP) : Address(AF_INET, pro) {
        addr_.sa4.sin_addr.s_addr = inet_addr(addr.c_str());
        addr_.sa4.sin_port        = htons(port);
        protocol_ = pro;
    }

    Address4(const Address4& addr) : Address(addr) { }

    void swap(Address4& rv) noexcept {
        Address::swap(rv);
    }

    void reset(const sockaddr_in_t& addr) {
        addr_.sa4 = addr;
    }

    void sethAddr(const in_addr4_t& addr) noexcept;

    void setnAddr(const in_addr4_t& addr) noexcept;

    void sethAddr(const in_addr4_t& addr, port_t port) noexcept;

    void setnAddr(const in_addr4_t& addr, port_t port) noexcept;

    void setAddr(const in_addr4_t& addr, port_t port) noexcept {
        sethAddr(addr, port);
    }

    void setAddr(const char* addr);

    void setAddr(const std::string& addr);

    void setPort(port_t port) noexcept {
        sethPort(port);
    }

    void sethPort(port_t port) noexcept;

    void setnPort(port_t port) noexcept;

    sockaddr_in_t get(void) const noexcept {
        return addr_.sa4;
    }

    port_t gethPort(void) const noexcept;

    port_t getnPort(void) const noexcept;

    port_t getPort(void) const noexcept {
        return gethPort();
    }
    
    in_addr4_t getnIPAddr(void) const noexcept;

    in_addr4_t gethIPAddr(void) noexcept;

    in_addr4_t getIPAddr(void) noexcept {
        return gethIPAddr();
    }

}; // class Address4


class Address6 : public Address {
public:
    Address6(void) : Address(AF_INET6) { }

    Address6(const port_t& port) : Address(AF_INET6) {
        addr_.sa6.sin6_port = port;
    }

}; // class Addr6


} // namespace tab;

#endif // __ADDRESS_HPP__