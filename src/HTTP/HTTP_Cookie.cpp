#include "EzNet/HTTP/HTTP_Cookie.hpp"
#include <iostream>
namespace tab {
namespace HTTP {

Cookie Cookie::parse(const std::string& raw) {
    Cookie ret;
    size_t i = 0;
    for (; i < raw.size() && raw[i] == ' '; ++i);
    for (; i < raw.size() && raw[i] != '='; ++i);
    if (i == 0 || i >= raw.size())
        throw InvalidCookieExpressionException();
    size_t pos_equal = i;
    ret.key_.assign(raw.begin(), raw.begin() + i);
    
    for (; i < raw.size() && raw[i] != ';'; ++i);
    ret.data_.value.assign(raw.begin() + pos_equal + 1, raw.begin() + i);
    ++i;
    for (; i < raw.size() && raw[i] == ' '; ++i);
    if (i == raw.size())
        return ret;

    while (i < raw.size()) {
        switch (raw[i]) {
            case 'e':
            case 'E': { // Expires
                for (; i < raw.size() && raw[i] != '='; ++i);
                ++i;
                size_t begin_expire = i;
                for (; i < raw.size() && raw[i] != ';'; ++i);
                size_t end_expire = i;
                if (begin_expire >= raw.size() || begin_expire == end_expire)
                    throw InvalidCookieExpressionException();
                ret.data_.expire.assign(raw.begin() + begin_expire,
                                        raw.begin() + end_expire);
                break;
            }
            case 's': // fallthrough
            case 'S': {
                if (i == raw.size() - 1)
                    throw InvalidCookieExpressionException();
                switch (raw[i+1]) {
                    case 'a': // SameSite
                        for (; i < raw.size() && raw[i] != '='; ++i);
                        ++i;
                        if (i >= raw.size())
                            throw InvalidCookieExpressionException();
                        if (raw[i] == 'S')
                            ret.data_.same_site = SameSite::Strict;
                        else if (raw[i] == 'L')
                            ret.data_.same_site = SameSite::Lax;
                        else
                            ret.data_.same_site = SameSite::None;
                        for (; i < raw.size() && raw[i] != ';'; ++i);
                        break;
                    case 'e': // Secure
                        ret.data_.secure = true;
                        for (; i < raw.size() && raw[i] != ';'; ++i);
                        break;
                    default:
                        throw InvalidCookieExpressionException();
                        break;
                }
                break;
            }
            case 'd': // fallthrough
            case 'D': { // Domain
                for (; i < raw.size() && raw[i] != '='; ++i);
                ++i;
                if (i >= raw.size())
                    throw InvalidCookieExpressionException();
                size_t begin_domain = i;
                for (; i < raw.size() && raw[i] != ';'; ++i);
                if (begin_domain >= i)
                    throw InvalidCookieExpressionException();
                ret.data_.domain.assign(raw.begin() + begin_domain, 
                                        raw.begin() + i);
                break;
            }
            case 'p': // fallthrough
            case 'P': { // Path
                for (; i < raw.size() && raw[i] != '='; ++i);
                ++i;
                if (i >= raw.size())
                    throw InvalidCookieExpressionException();
                size_t begin_path = i;
                for (; i < raw.size() && raw[i] != ';'; ++i);
                if (begin_path == i)
                    throw InvalidCookieExpressionException();
                ret.data_.path.assign(raw.begin() + begin_path, 
                                      raw.begin() + i);
                break;
            }
            case 'h': // fallthrough
            case 'H': { // HttpOnly
                for (; i < raw.size() && raw[i] != ';'; ++i);
                break;
            }
            case 'm': // fallthrough
            case 'M': { // Max-Age
                for (; i < raw.size() && raw[i] != '='; ++i);
                ++i;
                if (i >= raw.size())
                    throw InvalidCookieExpressionException();
                size_t begin_ma = i;
                for (; i < raw.size() && raw[i] != ';'; ++i);
                if (begin_ma == i)
                    throw InvalidCookieExpressionException();
                ret.data_.max_age.assign(raw.begin() + begin_ma, 
                                         raw.begin() + i);
                break;
            }
            default: {
                auto temp = i;
                for (; i < raw.size() && raw[i] != '='; ++i);
                std::string unknown_key(raw.begin() + temp, raw.begin() + i);
                temp = i;
                for (; i < raw.size() && raw[i] != ';'; ++i);
                std::pair<std::string, std::string> dat(
                    std::move(unknown_key),
                    std::move(
                        std::string(raw.begin() + temp, raw.begin() + i)));
                ret.data_.unknown.insert(std::move(dat));
                break;
            }
        }
        ++i;
        for (; i < raw.size() && raw[i] == ' '; ++i);
        if (i >= raw.size())
            return ret;
    }

    return ret;
} // Cookie::parse(const std::string&)


std::string Cookie::getString() const {
    std::string ret(key_);
    ret.push_back('=');
    ret.append(data_.value);
    if (!data_.domain.empty()) {
        ret.append("; Domain=");
        ret.append(data_.domain);
    }
    if (!data_.path.empty()) {
        ret.append("; Path=");
        ret.append(data_.path);
    }
    if (!data_.expire.empty()) {
        ret.append("; Expires=");
        ret.append(data_.expire);
    }
    if (!data_.max_age.empty()) {
        ret.append("; Max-Age=");
        ret.append(data_.max_age);
    }
    if (data_.same_site != SameSite::Null) {
        ret.append("; SameSite=");
        switch (data_.same_site) {
            case SameSite::Lax:
                ret.append("Lax");
                break;
            case SameSite::Strict:
                ret.append("Strict");
                break;
            case SameSite::None:
                ret.append("None");
                break;
            default:
                break;
        }
    }
    if (data_.secure) {
        ret.append("; Secure");
    }
    for (auto i : data_.unknown) {
        ret.append("; ");
        ret.append(i.first);
        ret.append("=");
        ret.append(i.second);
    }
    return ret;
}


Cookie Cookies::at(const std::string& key) const {
    auto ite = cookie_map_.find(key);
    Cookie ret;
    if (ite != cookie_map_.end()) {
        ret.data_ = ite->second;
        ret.key_ = ite->first;
    }
    return ret;
}

Cookies& Cookies::add(const Cookie& c) {
    cookie_map_.insert({c.key_, c.data_});
    return *this;
}

Cookies& Cookies::add(Cookie&& c) {
    cookie_map_.insert(
        std::move(
            std::pair<std::string, Cookie::fields>(
                std::move(c.key_), std::move(c.data_))));
    return *this;
}

Cookies& Cookies::remove(const std::string& key) {
    auto ite = cookie_map_.find(key);
    if (ite != cookie_map_.end())
        cookie_map_.erase(ite);    
    return *this;
}

std::string Cookies::getSettingString() const {
    std::string ret;
    for (auto i : cookie_map_) {
        Cookie c;
        c.key_ = i.first;
        c.data_ = i.second;
        ret.append("Set-Cookie: ");
        ret.append(c.getString());
        ret.append("\r\n");
    }
    return ret;
}

std::string Cookies::getUploadString() const {
    std::string ret("Cookie: ");
    size_t cnt = 0;
    for (auto i : cookie_map_) {
        if (cnt != 0)
            ret.append("; ");
        ret.append(i.first);
        ret.append("=");
        ret.append(i.second.value);
        ++ cnt;
    }
    ret.append("\r\n");
    return ret;
}

} // namespace HTTP
} // namespace tab