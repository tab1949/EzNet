#ifndef __URL_HPP__
#define __URL_HPP__

#include <string>
#include <utility>

#include "EzNet/Utility/Network/Address.hpp"

namespace tab {

class Host {
public:
    Host(void) { }

    Host(const Host& host) : addr_(host.addr_) { }

    Host(const Address& host) : addr_(host) { }

    Host(const std::string& host, protocol_t proto = IPPROTO_TCP);

    Host& set(const std::string& host);
    
    Host& set(const Address& host) {
        addr_ = host;
        return *this;
    }

    Host& operator=(const std::string& s) {
        set(s);
        return *this;
    }

    Host& operator=(const Address& a) {
        set(a);
        return *this;
    }

    Host& setPort(port_t p) {
        addr_.setPort(p);
        return *this;
    }

    std::string getStr(void);

    Address getAddr(void) const noexcept {
        return addr_;
    }

    operator Address() {
        return addr_;
    }

    bool operator==(const Host& h) {
        return addr_ == h.addr_;
    }

    bool operator!=(const Host& h) {
        return !operator==(h);
    }
    
protected:
    Address addr_;

}; // class Host


class URL {
public:
    enum class Protocol {
        NONE   = 0,
        HTTP   = 1,
        HTTPS  = 2,
        FTP    = 3,
        SSH    = 4
    };

    using path_t = std::string;
    using Path   = path_t;
    
public:
    URL(const URL& val) : 
        proto_(val.proto_), 
        port_ (val.port_), 
        port_default_(val.port_default_), 
        hostname_(val.hostname_), 
        path_    (val.path_) { }

    URL(URL&& rv) noexcept {
        operator=(std::move(rv));
    }

    URL(void) noexcept : 
        proto_(Protocol::NONE),
        port_(0),
        port_default_(true) {
    }

    URL(const std::string& url)
        : URL() {
        this->set(url);
    }

    virtual void swap(URL& v) noexcept {
        std::swap(proto_, v.proto_);
        std::swap(port_, v.port_);
        std::swap(path_, v.path_);
        std::swap(port_default_, v.port_default_);
        hostname_.swap(v.hostname_);
    }

    URL& operator=(URL&&) = default;
    URL& operator=(const URL&) = default;

    static URL parse(const std::string& url);

    Protocol getProtocol(void) const noexcept {
        return proto_;
    }

    Host getHost(void) const;

    std::string getHostName(void) const {
        return hostname_;
    }

    port_t getPort(void) const noexcept {
        return port_;
    }

    path_t getPath(void) const noexcept {
        return path_;
    }

    std::string getStr(void);

    operator Host() const {
        return this->getHost();
    }

    operator Address() const {
        return this->getHost().getAddr();
    }

    operator std::string() {
        return this->getStr();
    }

    bool set(const std::string& url);

    bool setProtocol(Protocol proto) noexcept {
        proto_ = proto;
        return true;
    }

    bool setHost(const std::string& hostname) noexcept {
        hostname_ = hostname;
        return true;
    }

    bool setHost(const std::string& hostname, port_t port) noexcept;

    bool setPort(port_t port);

    bool setPath(const std::string& path);

    bool empty(void) {
        return hostname_.empty();
    }

    bool isDefaultPort(void) const noexcept {
        return port_default_;
    }

protected:
    Protocol    proto_;
    port_t      port_;
    bool        port_default_;
    std::string hostname_;
    path_t      path_;

}; // class URL

} // namespace tab

#endif // __URL_HPP__