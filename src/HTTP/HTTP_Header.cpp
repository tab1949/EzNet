#include <exception>
#include <stdexcept>

#include "EzNet/HTTP/HTTP_Header.hpp"

namespace tab {

namespace HTTP {
    
const char* HeaderKeyName[]{
    "",
    "Accept",
    "Accept-CH",
    "Accept-Charset",
    "Accept-Features",
    "Accept-Encoding",
    "Accept-Language",
    "Accept-Ranges",
    "Access-Control-Allow-Credentials",
    "Access-Control-Allow-Origin",
    "Access-Control-Allow-Methods",
    "Access-Control-Allow-Headers",
    "Access-Control-Expose-Headers",
    "Access-Control-Max-Age",
    "Access-Control-Request-Method",
    "Access-Control-Request-Headers",
    "Age",
    "Allow",
    "Also-Control",
    "Alternate-Recipient",
    "Approved",
    "Archive",
    "Archived-At",
    "Authorization",
    "Base",
    "Bcc",
    "Body",
    "Cache-Control",
    "Cancel-Key",
    "Cancel-Lock",
    "Cc",
    "Comments",
    "Connection",
    "Content-Alternative",
    "Content-Base",
    "Content-Description",
    "Content-Disposition",
    "Content-Duration",
    "Content-Encoding",
    "Content-Language",
    "Content-Length",
    "Content-Location",
    "Content-Range",
    "Content-Security-Policy",
    "Content-Type",
    "Cookie",
    "Date",
    "ETag",
    "Expect",
    "Expires",
    "From",
    "Host",
    "If-Match",
    "If-Modified-Since",
    "If-None-Match",
    "If-Range",	
    "If-Unmodified-Since",
    "Last-Modified",
    "Link",
    "Location",
    "Max-Forwards",
    "Negotiate",
    "Origin",
    "Pragma",
    "Proxy-Authenticate",		
    "Proxy-Authorization",			
    "Range",
    "Referer",
    "Sec-Websocket-Extensions",
    "Sec-Websocket-Key",
    "Sec-Websocket-Origin",
    "Sec-Websocket-Protocol",
    "Sec-Websocket-Version",
    "Server",
    "Set-Cookie",
    "Set-Cookie2",
    "Strict-Transport-Security",
    "TCN",
    "TE",
    "Trailer",
    "Transfer-Encoding",
    "Upgrade",
    "User-Agent",
    "Variant-Vary",
    "Vary",
    "Via",
    "Warning",
    "WWW-Authenticate"
};

constexpr static size_t HeaderFieldCount 
    = sizeof(HeaderKeyName) / sizeof(const char*);

HeaderFieldName HeaderFieldStringToEnum(const std::string& str) {
    const char** pos 
        = std::find(HeaderKeyName, HeaderKeyName + HeaderFieldCount, str);
    if (pos != HeaderKeyName + HeaderFieldCount)
        return HeaderFieldName(pos - HeaderKeyName);
    else
        return NONE;
}

class NullHeader : public HeaderBase {
public:
    virtual Type type() const noexcept {
        return Type::Null;
    }

    virtual bool operator==(const HeaderBase& header) const noexcept {
        return header.type() == Type::Null;
    }

    virtual size_t size() const {
        return 0;
    }

    virtual void setValue(const std::string&) { }
    virtual void setValue(std::string&&) noexcept { }

    virtual std::string getKeyString() const { return std::string(); }
    virtual std::string getValue() const { return std::string(); }
    virtual std::string getString() const { return std::string(); }

};

static NullHeader null_header; 

Header::Header() : ptr_(&null_header) {

}

Header::Header(const Header& h) {
    switch(h.type()) {
        case Type::Null: 
            ptr_ = &null_header; break;
        case Type::Common: 
            ptr_ = new CommonHeader(*(CommonHeader*)(void*)&h); break;
        case Type::Unknown: 
            ptr_ = new UnknownHeader(*(UnknownHeader*)(void*)&h); break;
        default: 
            throw InvalidValueException(
                "Header::Header(const Header&): Error header type.");
    }
}

Header::Header(Header&& h) {
    ptr_ = h.ptr_;
    h.ptr_ = &null_header;
}

Header::Header(const std::string& k) {
    auto key = HeaderFieldStringToEnum(k);
    if (key != NONE)
        ptr_ = new CommonHeader(key, "");
    else
        ptr_ = new UnknownHeader(k, "");
}

Header::Header(const std::string& k, const std::string& v) {
    auto key = HeaderFieldStringToEnum(k);
    if (key != NONE)
        ptr_ = new CommonHeader(key, v);
    else
        ptr_ = new UnknownHeader(k, v);
}

Header::Header(HeaderFieldName k) {
    ptr_ = new CommonHeader(k, "");
}

Header::Header(HeaderFieldName k, const std::string& v) {
    ptr_ = new CommonHeader(k, v);
}

Header::~Header() {
    if (ptr_ != &null_header)
        delete ptr_;
}

Header Header::parse(const char* raw, size_t len) {
    size_t i = 0;
    Header ret;

    for (; i < len; ++i)
        if (raw[i] == ':')
            break;
    std::string key_name(raw, raw + i);
    auto header_name = HeaderFieldStringToEnum(key_name);
    std::string* value_ptr = nullptr;
    if (header_name != NONE) {
        auto tmp = new CommonHeader(header_name, std::string());
        ret.ptr_ = tmp;
        value_ptr = &tmp->value_;
        key_name.clear();
    }
    else {
        auto ptr = new UnknownHeader(std::move(key_name), std::string());
        value_ptr = &ptr->value_;
        ret.ptr_ = ptr;
        key_name.clear();
    }
    
    for (++i; i < len; ++i)
        if (raw[i] != ' ')
            break;
    size_t offset = 0;
    for (size_t j = len; j >= i; --j)
        if (raw[j] == '\r')
            offset = len - j;
    *value_ptr = std::move(std::string(raw + i, raw + len - offset)); 

    return ret;
}

void Header::setValue(const std::string& val) {
    if (ptr_ != &null_header)
        ptr_->setValue(val);
}

void Header::setValue(std::string&& val) noexcept {
    if (ptr_ != &null_header)
        ptr_->setValue(std::move(val));
}

std::string CommonHeader::getString(void) const {
    std::string ret;
    ret.append(HeaderKeyName[key_]);
    ret.append(": ");
    ret.append(value_);
    ret.append("\r\n");
    return ret;
}

std::string Headers::getStr(void) const {
    try {
        std::string ret;
        for (auto i = map_common_.begin(); i != map_common_.end(); ++i)
            ret.append(
                std::string(HeaderKeyName[i->first])
                 + ": " + i->second + "\r\n");
        for (auto i = map_unknown_.begin(); i != map_unknown_.end(); ++i)
            ret.append(i->first + ": " + i->second + "\r\n");
        return ret;
    }
    catch (...) {
        return "";
    }
}

Headers& Headers::addHeader(const Header& header) {
    if (header.type() == Header::Type::Common) {
        auto&& ref = static_cast<const CommonHeader&>(*header.ptr_);
        if (ref.getKey() == NONE)
            return *this;

        auto&& ite = map_common_.find(ref.getKey());
        if (ite != map_common_.end())
            ite->second = ref.getValue();
        else
            map_common_.insert(ref.getPair());
    }
    else if (header.type() == Header::Type::Unknown) {
        auto&& ref = static_cast<const UnknownHeader&>(*header.ptr_);
        if (ref.getKey().empty())
            return *this;

        auto&& ite = map_unknown_.find(ref.getKey());
        if (ite != map_unknown_.end())
            ite->second = ref.getValue();
        else
            map_unknown_.insert({ref.getKey(), ref.getValue()});
    }
    else {
        throw std::logic_error("");
    }
    return *this;
}

Headers& Headers::addHeader(Header&& header) {
    if (header.type() == Header::Type::Common) {
        auto&& ref = std::move(header.to<CommonHeader>());
        if (ref.getKey() == NONE)
            return *this;

        auto&& ite = map_common_.find(ref.getKey());
        if (ite != map_common_.end())
            ite->second = std::move(ref.value_);
        else
            map_common_.insert(
                std::move(std::pair<HeaderFieldName, std::string>({
                    ref.key_, std::move(ref.value_)})));
    }
    else if (header.type() == Header::Type::Unknown) {
        auto&& ref = std::move(header.to<UnknownHeader>());
        if (ref.getKey().empty())
            return *this;
            
        auto&& ite = map_unknown_.find(ref.getKey());
        if (ite != map_unknown_.end())
            ite->second = std::move(ref.value_);
        else
            map_unknown_.insert(std::move(std::pair<std::string, std::string>({
                std::move(ref.key_), std::move(ref.value_)})));
    }
    else {
        throw std::logic_error("");
    }
    return *this;
}

Headers& Headers::addHeader(HeaderFieldName key, const std::string& value) {
    if (key == NONE)
        return *this;

    auto&& ite = map_common_.find(key);
    if (ite != map_common_.end())
        ite->second = value;
    else
        map_common_.insert({key, value});
    
    return *this;
}

Headers& Headers::addHeader(const std::string& key, const std::string& value) {
    if (key.empty()) 
        return *this;
    auto header_name = HeaderFieldStringToEnum(key);
    if (header_name != NONE) {
        auto&& ite = map_common_.find(header_name);
        if (ite != map_common_.end())
            ite->second = value;
        else
            map_common_.insert({header_name, value});
    }
    else {
        auto&& ite = map_unknown_.find(key);
        if (ite != map_unknown_.end())
            ite->second = value;
        else
            map_unknown_.insert({key, value});
    }
    
    return *this;
}

std::string Headers::find(HeaderFieldName key) {
    if (key == NONE)
        return std::string();
    auto&& ite = map_common_.find(key);
    if (ite != map_common_.end())
        return ite->second;
    else
        return std::string();
}

std::string Headers::find(const std::string& key) {
    auto header_name = HeaderFieldStringToEnum(key);
    if (header_name != NONE) {
        auto&& ite = map_common_.find(header_name);
        if (ite != map_common_.end())
            return ite->second;
        else
            return std::string();
    }
    else {
        auto&& ite = map_unknown_.find(key);
        if (ite != map_unknown_.end())
            return ite->second;
        else
            return std::string();
    }
}

Headers& Headers::remove(const HeaderFieldName& key) {
    if (key == NONE)
        return *this;
    auto&& ite = map_common_.find(key);
    if (ite != map_common_.end())
        map_common_.erase(ite);
    return *this;
}

Headers& Headers::remove(const std::string& key) {
    auto header_name = HeaderFieldStringToEnum(key);
    if (header_name != NONE) {
        auto&& ite = map_common_.find(header_name);
        if (ite != map_common_.end())
            map_common_.erase(ite);
    }
    else {
        auto&& ite = map_unknown_.find(key);
        if (ite != map_unknown_.end())
            map_unknown_.erase(ite);
    }
    return *this;
}

} // namespace HTTP

} // namespace tab