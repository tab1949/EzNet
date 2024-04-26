#ifndef __HTTP_HEADER_HPP__
#define __HTTP_HEADER_HPP__

#include <algorithm>
#include <cstring>
#include <exception>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "EzNet/Utility/General/Exceptions.hpp"
#include "HTTP_Cookie.hpp"

namespace tab {

namespace HTTP {

class Header;
class HeaderBase;
class CommonHeader;
class UnknownHeader;

/**
 * @brief Enumeration of HTTP headers.
 * 
 * @note See 
 *       https://www.iana.org/assignments/message-headers/message-headers.xhtml
 *       for the latest content.
 * 
 */
enum HeaderFieldName {
    NONE = 0,
    ACCEPT,
    ACCEPT_CH,
    ACCEPT_CHARSET,
    ACCEPT_FEATURES,
    ACCEPT_ENCODING,
    ACCEPT_LANGUAGE,
    ACCEPT_RANGES,
    ACCESS_CONTROL_ALLOW_CREDENTIALS,
    ACCESS_CONTROL_ALLOW_ORIGIN,
    ACCESS_CONTROL_ALLOW_METHODS,
    ACCESS_CONTROL_ALLOW_HEADERS,
    ACCESS_CONTROL_EXPOSE_HEADERS,
    ACCESS_CONTROL_MAX_AGE,
    ACCESS_CONTROL_REQUEST_METHOD,
    ACCESS_CONTROL_REQUEST_HEADERS,
    AGE,
    ALLOW,
    ALSO_CONTROL,
    ALTERNATE_RECIPIENT,
    APPROVED,
    ARCHIVE,
    ARCHIVED_AT,
    AUTHORIZATION,
    BASE,
    BCC,
    BODY,
    CACHE_CONTROL,
    CANCEL_KEY,
    CANCEL_LOCK,
    CC,
    COMMENTS,
    CONNECTION,
    CONTENT_ALTERNATIVE,
    CONTENT_BASE,
    CONTENT_DESCRIPTION,
    CONTENT_DISPOSITION,
    CONTENT_DURATION,
    CONTENT_ENCODING,
    CONTENT_LANGUAGE,
    CONTENT_LENGTH,
    CONTENT_LOCATION,
    CONTENT_RANGE,
    CONTENT_SECURITY_POLICY,
    CONTENT_TYPE,
    COOKIE,
    DATE,
    ETAG,
    EXPECT,
    EXPIRES,
    FROM,
    HOST,
    IF_MATCH,
    IF_MODIFIED_SINCE,
    IF_NONE_MATCH,
    IF_RANGE,
    IF_UNMODIFIED_SINCE,
    LAST_MODIFIED,
    LINK,
    LOCATION,
    MAX_FORWARDS,
    NEGOTIATE,
    ORIGIN,
    PRAGMA,
    PROXY_AUTHENTICATE,
    PROXY_AUTHORIZATION,
    RANGE,
    REFERER,
    SEC_WEBSOCKET_EXTENSIONS,
    SEC_WEBSOCKET_KEY,
    SEC_WEBSOCKET_ORIGIN,
    SEC_WEBSOCKET_PROTOCOL,
    SEC_WEBSOCKET_VERSION,
    SERVER,
    SET_COOKIE,
    SET_COOKIE2,
    STRICT_TRANSPORT_SECURITY,
    TCN,
    TE,
    TRAILER,
    TRANSFER_ENCODING,
    UPGRADE,
    USER_AGENT,
    VARIANT_VARY,
    VARY,
    VIA,
    WARNING,
    WWW_AUTHENTICATE
};

/**
 * @brief C-style string of HTTP headers.
 * @brief Use the value in 'enum HeaderFieldName' to access it.
 * 
 * @note Defined in HTTP_Header.cpp
 */
extern const char* HeaderKeyName[];

/**
 * @warning This interface is not recommended to use directly.
 */
class HeaderBase {
public:
    enum class Type {Null = 0, Common, Unknown};

public:
    virtual ~HeaderBase() { }
    virtual Type type() const noexcept = 0;

    virtual bool operator==(const HeaderBase& header) const noexcept = 0;

    virtual size_t size() const = 0;

    virtual void setValue(const std::string& val) = 0;
    virtual void setValue(std::string&& val) noexcept = 0;

    virtual std::string getKeyString() const = 0;
    virtual std::string getValue() const = 0;
    virtual std::string getString() const = 0;

}; // class HeaderBase


/**
 * @brief A CommonHeader stores the "key" of a header by 'HeaderFieldName',
 * @brief which is designed to reduce memory cost, and this is the difference
 * @brief between it and UnknownHeader.
 */
class CommonHeader : public HeaderBase {
public:
    constexpr static Type type_v = Type::Common;

public:
    CommonHeader(HeaderFieldName key, const std::string& val) : 
        key_(key), 
        value_(val) {
    } 

    CommonHeader(const std::pair<HeaderFieldName, std::string>& p) :
        key_(p.first),
        value_(p.second) {
    }

    CommonHeader(const CommonHeader& header) : 
        key_(header.key_), 
        value_(header.value_) {
    }

    CommonHeader(CommonHeader&& header) noexcept : 
        key_(std::move(header.key_)), 
        value_(std::move(header.value_)) {
    }

    virtual Type type() const noexcept override {
        return Type::Common;
    }

    CommonHeader& operator=(const CommonHeader& header) {
        key_ = header.key_;
        value_ = header.value_;
        return *this;
    }

    CommonHeader& operator=(CommonHeader&& header) noexcept {
        key_ = header.key_;
        value_.assign(std::move(header.value_));
        return *this;
    }

    CommonHeader& swap(CommonHeader& header) noexcept {
        std::swap(key_, header.key_);
        value_.swap(header.value_);
        return *this;
    }

    bool operator==(const HeaderBase& header) const noexcept override {
        if (header.type() != Type::Common)
            return false;
        auto&& ref = static_cast<const CommonHeader&>(header);
        return (key_ == ref.key_ && value_ == ref.value_);
    }

    size_t size() const override {
        // What's "+4" meaning: 
        // ": " and "\r\n"
        return std::strlen(HeaderKeyName[key_]) + value_.size() + 4;
    }

    void setKey(HeaderFieldName key) noexcept {
        key_ = key;
    }

    void setValue(const std::string& val) noexcept override {
        value_ = val;
    }

    void setValue(std::string&& val) noexcept override {
        value_ = std::move(val);
    }

    HeaderFieldName getKey() const {
        return key_;
    }

    std::string getKeyString() const override {
        return HeaderKeyName[key_];
    }

    std::string getValue() const override {
        return value_;
    }

    std::pair<HeaderFieldName, std::string> getPair() const {
        return {key_, value_};
    }

    /**
     * @brief Convert a CommonHeader into std::pair.
     * 
     * @return std::pair<HeaderFieldName, std::string> 
     */
    operator std::pair<HeaderFieldName, std::string>() {
        return std::move(this->getPair());
    }

    std::string getString() const override;

protected:
    HeaderFieldName key_;
    std::string value_;

    friend class Header;
    friend class Headers;

}; // class CommonHeader


/**
 * @brief A 'UnknownHeader' stores the "key" of a header in 'std::string'.
 *        It can store any headers, although the header doesn't exist in 
 *        'HeaderFieldName', and this is the difference between 
 *        'UnknownHeader' and 'CommonHeader'. It may take more space.
 * 
 * @note To reduce memory cost, use 'CommonHeader'.
 */
class UnknownHeader : public HeaderBase {
public:
    constexpr static Type type_v = Type::Unknown;

public:
    template <class TK, class TV>
    UnknownHeader(TK k, TV v) : 
        key_(std::forward<TK>(k)),
        value_(std::forward<TV>(v)) { }

    virtual Type type() const noexcept override {
        return Type::Unknown;
    }

    virtual bool operator==(const HeaderBase& header) const noexcept override {
        if (header.type() != Type::Unknown)
            return false;
        auto&& ref = static_cast<const UnknownHeader&>(header);
        return (key_ == ref.key_ && value_ == ref.value_);
    }

    virtual size_t size() const override {
        return key_.size() + value_.size() + 4;
    }

    virtual void setValue(const std::string& val) override {
        value_ = val;
    }

    virtual void setValue(std::string&& val) noexcept override {
        value_ = std::move(val);
    }

    std::string getKey() const {
        return key_;
    }

    virtual std::string getKeyString() const override {
        return key_;
    }

    virtual std::string getValue() const override {
        return value_;
    }

    virtual std::string getString() const override {
        return key_ + ": " + value_ + "\r\n";
    }
    
protected:
    std::string key_;
    std::string value_;

    friend class Header;
    friend class Headers;

}; // class UnknownHeader
    

/**
 * @brief HTTP header class.
 */
class Header {
public:
    using Type = HeaderBase::Type;

public:
    Header();

    Header(const std::string& k);

    Header(const std::string& k, const std::string& v);

    Header(HeaderFieldName k);

    Header(HeaderFieldName k, const std::string& v);

    Header(const Header& h);

    Header(Header&& h);

    ~Header();
    
    Type type() const noexcept {
        return ptr_->type();    
    }

    bool operator==(const Header& header) const noexcept {
        return ptr_->operator==(*header.ptr_);
    }

    size_t size() const {
        return ptr_->size();
    }

    template <class T>
    T& to() {
        static_assert(std::is_base_of<HeaderBase, T>::value, 
                      "The template argument 'T' is not derived from 'HTTP::HeaderBase'.");
        if (ptr_->type() != T::type_v)
            throw std::runtime_error(
                  "tab::HTTP::Header::to<T>(): This object is not actually of type 'T'.");    
        return *static_cast<T*>(ptr_); 
    }

    void setValue(const std::string& val);

    void setValue(std::string&& val) noexcept;

    std::string getKeyString() const {
        return std::move(ptr_->getKeyString());
    }

    std::string getValue() const {
        return std::move(ptr_->getValue());
    }

    std::string getString() const {
        return std::move(ptr_->getString());
    }

    static Header parse(const char* raw, size_t len);
    
    static Header parse(const std::string& raw) {
        return std::move(parse(raw.c_str(), raw.size()));
    }

    operator std::string() const {
        return getString();
    }

private:
    HeaderBase* ptr_;

    friend class Headers;

}; // class Header


/**
 * @brief A set of 'Header's.
 */
class Headers {
public:
    Headers(void) = default;

    Headers(const Headers&) = default;

    Headers(Headers&& val) noexcept:
        map_common_(std::move(val.map_common_)),
        map_unknown_(std::move(val.map_unknown_)) { }

    size_t size(void) const noexcept{
        return map_common_.size() + map_unknown_.size();
    }

    std::string& operator[](HeaderFieldName key) {
        return map_common_.operator[](key);
    }

    Headers& operator=(const Headers& val) {
        map_common_ = val.map_common_;
        map_unknown_ = val.map_unknown_;
        return *this;
    }

    Headers& operator=(Headers&& val) {
        map_common_ = std::move(val.map_common_);
        map_unknown_ = std::move(val.map_unknown_);
        return *this;
    }

    std::string getStr(void) const;

    Headers& addHeader(const Header& header);
    Headers& addHeader(Header&& header);
    Headers& addHeader(HeaderFieldName key, const std::string& value);
    Headers& addHeader(const std::string& key, const std::string& value);
    
    /**
     * @brief It's same as addHeader().
     */
    Headers& operator+=(const Header& header) noexcept {
        this->addHeader(header);
        return *this;
    }

    /**
     * @brief It's same as addHeader().
     */
    Headers& operator+=(Header&& header) noexcept {
        this->addHeader(std::move(header));
        return *this;
    }

    /**
     * @brief Find the value of the key.
     * 
     * @return Return a empty string if the key is not found, otherwise it will 
     *         return the existing value.
     */
    std::string find(HeaderFieldName key);
    std::string find(const std::string& key);

    Headers& remove(const HeaderFieldName& key);
    Headers& remove(const std::string& key);
    
private: 
    // Holds all the keys and values.
    std::map<HeaderFieldName, std::string> map_common_;
    std::map<std::string, std::string> map_unknown_;

}; // class Headers


} // namespace HTTP

} // namespace tab

#endif // __HTTP_HEADER_HPP__