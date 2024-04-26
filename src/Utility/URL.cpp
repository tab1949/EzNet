#include "EzNet/Utility/Network/URL.hpp"

namespace tab {


Host::Host(const std::string& host, protocol_t proto) {
    set(host);
    addr_.setProtocol(proto);
}

Host& Host::set(const std::string& host) {
    size_t i = 0;
    // Find out ':'.
    for (; i < host.size(); ++i) {
        if (host[i] == ':') {
            ++i;
            break;
        }
    }

    if (i == host.size()) { // Port is not specified.
        addr_.setAddrByName(host);
    }
    else {
        addr_.setAddrByName(std::string(host.begin(), host.begin() + i - 1));
        addr_.sethPort(static_cast<port_t>(
            std::stoi(std::string(host.begin() + i, host.end()))));
    }

    return *this;
}

std::string Host::getStr(void) {
    if (addr_.getAF() == AF_INET)
        return addr_.getStr() + ':' + std::to_string(addr_.gethPort());
    else if (addr_.getAF() == AF_INET6)
        return (
            std::string("[")
            + addr_.getStr() 
            + "]:" 
            + std::to_string(addr_.gethPort())
        );
    return "";
}

bool URL::set(const std::string& url) {
    if (url.empty())
        return false;
    // Get the protocol in URL.
    std::string temp;
    size_t i = 0, lim = url.size();
    for (; i < lim; ++i) {
        if (url.at(i) != ':') {
            temp.push_back(url.at(i));
        }
        else {
            break ;
        }
    }
    proto_ = Protocol::NONE;
    port_ = 0;
    if (i == lim) { // This is a uri.
        path_ = url;
        return true;
    }
    if (temp == "ssh") {
        proto_ = Protocol::SSH;
        port_  = 22;
    }
    else if (temp == "http") {
        proto_ = Protocol::HTTP;
        port_  = 80;
    }
    else if (temp == "https") {
        proto_ = Protocol::HTTPS;
        port_  = 443;
    }
    temp.clear();

    // Get the host in URL.
    for (i += 3; i < lim; ++i) {
        if (url.at(i) != '/') {
            if (url.at(i) == ':') {
                std::string port;
                for (++i; i < lim; ++i) {
                    if ('0' <= url.at(i) && url.at(i) <= '9') {
                        port.push_back(url.at(i));
                    }
                    else {
                        break;
                    }
                }
                port_default_ = false;
                port_ = (port_t)std::stoi(port);
                break;
            }
            temp.push_back(url.at(i));
        }
        else {
            break;
        }
    }
    if (!temp.empty()) {
        hostname_.swap(temp);
    }
    else {
        return false;
    }
    temp.clear();

    // Get the path in URL.
    path_.assign(url.begin() + i, url.end());
    if (path_.empty())
        path_ = '/';
    
    return true;
}

std::string URL::getStr(void) {
    std::string temp;
    switch(proto_) {
        case Protocol::HTTP: 
            temp.append("http://");
            break;
        case Protocol::HTTPS:
            temp.append("https://");
            break;
        case Protocol::FTP:
            temp.append("ftp://");
            break;
        default:
            return temp;
    }
    temp.append(hostname_);
    if (!port_default_)
        temp.append(":" + std::to_string(port_));
    temp.append(path_);
    return temp;
}

URL URL::parse(const std::string& url) {
    return URL(url);
}

Host URL::getHost(void) const {
    if (!hostname_.empty()) {
        Address&& temp = Address::GetAddrByName(hostname_);
        temp.setPort(port_);
        return Host(temp);
    }
    else {
        return Host();
    }
}

bool URL::setHost(const std::string& hostname, port_t port) noexcept {
    hostname_ = hostname;
    setPort(port);
    return true;
}

bool URL::setPort(port_t port) {
    port_ = port;
    port_default_ = false;
    return true;
}

bool URL::setPath(const std::string& path) {
    path_ = path;
    return true;
}

} // namespace tab