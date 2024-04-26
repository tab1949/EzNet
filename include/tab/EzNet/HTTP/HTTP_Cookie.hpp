#ifndef __HTTP_COOKIE_HPP__
#define __HTTP_COOKIE_HPP__

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

namespace tab {
namespace HTTP {

class Cookie {
public:
    enum class SameSite {
        Null = 0,
        Lax,
        Strict,
        None
    };

    class InvalidCookieExpressionException : public std::exception {
    public:
        InvalidCookieExpressionException() = default;
        const char* what() const noexcept override {
            return "Invalid cookie expression";
        }
    };

public:
    /**
     * @brief Default constructor, does nothing.
     */
    Cookie() = default;

    Cookie(const Cookie& c) : 
        key_(c.key_),
        data_(c.data_) { }

    Cookie(Cookie&& c) : 
        key_(std::move(c.key_)),
        data_(std::move(c.data_)) { }

    Cookie(const std::string& raw) : Cookie(std::move(parse(raw))) { }

    Cookie& operator=(const Cookie& c) {
        key_  = c.key_;
        data_ = c.data_;
        return *this;
    }

    Cookie& operator=(Cookie&& c) {
        key_  = std::move(c.key_);
        data_ = std::move(c.data_);
        return *this;
    }

    /**
     * @brief Parse a string into an object of 'Cookie'
     * 
     * @param raw The raw string, the content after "Set-Cookie: " or "Cookie: "
     *            (!IMPORTANT! only one equation allowed in the second case, 
     *            e.g., string "a=b" in "Cookie: a=b" is okay but 
     *            "a=b; c=d" in "Cookie: a=b; c=d" is not. 
     *            So you need to pay attention when parsing such a expression.)
     * @return (Cookie) Result
     * @exception InvalidCookieExpressionException
     */
    static Cookie parse(const std::string& raw);

    /**
     * @brief Get the data, like "key=value"
     */
    std::string getData() const {
        return key_ + '=' + data_.value;
    }

    /**
     * @brief Get the key, the content before '='.
     */
    std::string getKey() const {
        return key_;
    }

    /**
     * @brief Get the value, the content after '='.
     */
    std::string getValue() const {
        return data_.value;
    }

    /**
     * @brief Get the content after "Domain=".
     */
    std::string getDomain() const {
        return data_.domain;
    }

    /**
     * @brief Get the content after "Path=".
     */
    std::string getPath() const {
        return data_.path;
    }

    /**
     * @brief Get the content after "Expires=".
     */
    std::string getExpire() const {
        return data_.expire;
    }

    /**
     * @brief Get the configuration of "SameSite".
     */
    SameSite getSameSite() const {
        return data_.same_site;
    }

    /**
     * @brief Get the string of this cookie.
     */
    std::string getString() const;

    /**
     * @brief Whether 'Secure' is set.
     */
    bool isSecureSet() {
        return data_.secure;
    }

    Cookie& setKey(const std::string& k) {
        key_ = k;
        return *this;
    }

    Cookie& setKey(std::string&& k) {
        key_ = std::move(k);
        return *this;
    }

    Cookie& setValue(const std::string& v) {
        data_.value = v;
        return *this;
    }

    Cookie& setValue(std::string&& v) {
        data_.value = std::move(v);
        return *this;
    }

    Cookie& setDomain(const std::string& d) {
        data_.domain = d;
        return *this;
    }

    Cookie& setDomain(std::string&& d) {
        data_.domain = std::move(d);
        return *this;
    }

    Cookie& setPath(const std::string& p) {
        data_.path = p;
        return *this;
    }

    Cookie& setPath(std::string&& p) {
        data_.path = std::move(p);
        return *this;
    }

    Cookie& setExpire(const std::string& e) {
        data_.expire = e;
        return *this;
    }

    Cookie& setExpire(std::string&& e) {
        data_.expire = std::move(e);
        return *this;
    }

    Cookie& setSameSite(SameSite ss) {
        data_.same_site = ss;
        return *this;
    }

    Cookie& setSecure(bool s) {
        data_.secure = s;
        return *this;
    }

private:
    std::string key_;
    struct fields {
        fields() : same_site(SameSite::Null), secure(false) { }

        fields(const fields& f) :  
            value(f.value),
            domain(f.domain), 
            path(f.path), 
            expire(f.expire), 
            max_age(f.max_age),
            same_site(f.same_site), 
            secure(f.secure),
            unknown(f.unknown) { }
        
        fields(fields&& f) : 
            value(std::move(f.value)),
            domain(std::move(f.domain)),
            path(std::move(f.path)),
            expire(std::move(f.expire)),
            max_age(std::move(f.max_age)),
            same_site(f.same_site),
            secure(f.secure),
            unknown(std::move(f.unknown)) { }

        void operator=(const fields& f) {
            value     = f.value;
            domain    = f.domain;
            path      = f.path;
            expire    = f.expire;
            same_site = f.same_site;
            secure    = f.secure;
            max_age   = f.max_age;
            unknown   = f.unknown;
        }
        
        void operator=(fields&& f) {
            value     = std::move(f.value);
            domain    = std::move(f.domain);
            path      = std::move(f.path);
            expire    = std::move(f.expire);
            max_age   = std::move(f.max_age);
            same_site = f.same_site;
            secure    = f.secure;
            unknown   = std::move(f.unknown);
        }

        std::string value;
        std::string domain;
        std::string path;
        std::string expire;
        std::string max_age;
        SameSite same_site;
        bool secure;

        std::map<std::string, std::string> unknown;
    } data_;

    // HttpOnly is not supported.

    friend class Cookies;

}; // class Cookie


/**
 * @brief A set of cookies.
 */
class Cookies {
public:
    Cookies() = default;
    Cookies(const Cookies& c) : cookie_map_(c.cookie_map_) { }
    Cookies(Cookies&& c) : cookie_map_(std::move(c.cookie_map_)) { }

    Cookies& operator=(const Cookies& c) {
        cookie_map_ = c.cookie_map_;
        return *this;
    }

    Cookies& operator=(Cookies&& c) {
        cookie_map_ = std::move(c.cookie_map_);
        return *this;
    }

    /**
     * @brief Find the cookie which has the same key as the given one.
     * 
     * @param key 
     * @return (Cookie) The cookie found.
     */
    Cookie at(const std::string& key) const;
    
    /**
     * @brief The alias of 'at(...)'.
     */
    Cookie get(const std::string& key) const {
        return at(key);
    }

    /**
     * @brief Same as 'at(...)'.
     */
    Cookie operator[](const std::string& key) const {
        return this->at(key);
    }
    
    /**
     * @brief Add a cookie to this set. (Copy)
     */
    Cookies& add(const Cookie& c);

    /**
     * @brief Add a cookie to this set. (Move)
     */
    Cookies& add(Cookie&& c);


    /**
     * @brief Merge a set of cookies into this set.
     */
    Cookies& add(const Cookies& c) {
        cookie_map_.insert(c.cookie_map_.begin(), c.cookie_map_.end());
        return *this;
    }

    /**
     * @brief Remove a cookie from this set.
     */
    Cookies& remove(const std::string& key);

    size_t size() const {
        return cookie_map_.size();
    }

    // TODO:
    // Cookies filter();

    std::string getSettingString() const;
    std::string getUploadString() const;
    
private:
    std::map<std::string, Cookie::fields> cookie_map_;

}; // class Cookies

} // namespace HTTP

} // namespace tab

#endif // __HTTP_COOKIE_HPP__