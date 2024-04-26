#ifndef __HTTP_REQUEST_HPP__
#define __HTTP_REQUEST_HPP__

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "HTTP_RequestLine.hpp"
#include "HTTP_Header.hpp"
#include "HTTP_Cookie.hpp"

#include "EzNet/Utility/Memory/Memory.hpp"

namespace tab {

/**
 * @brief Contains the data to send to request.
 * 
 */
class HttpRequest {
public:
    HttpRequest(HTTP::ReqMethod req, const HTTP::URI& uri, 
                const HTTP::ProtocolVersion& ver = "1.1") : 
        request_(req, uri, ver) { } 

    HttpRequest(const HTTP::RequestLine& rl) : request_(rl) { }

    HttpRequest(HTTP::RequestLine&& rl) : request_(std::move(rl)) { }

    virtual ~HttpRequest() noexcept { }

    HttpRequest(const HttpRequest& hr) :
        request_(hr.request_),
        headers_(hr.headers_),
        cookies_(hr.cookies_),
        body_(hr.body_) { }

    HttpRequest(HttpRequest&& hr) :
        request_(std::move(hr.request_)),
        headers_(std::move(hr.headers_)),
        cookies_(std::move(hr.cookies_)),
        body_(std::move(hr.body_)) { }

    HttpRequest& operator=(HttpRequest&& hr) {
        request_ = std::move(hr.request_);
        headers_ = std::move(hr.headers_);
        body_    = std::move(hr.body_);
        cookies_ = std::move(hr.cookies_);
        return *this;
    }

    HttpRequest& operator=(const HttpRequest& hr) {
        request_ = hr.request_;
        headers_ = hr.headers_;
        body_    = hr.body_;
        cookies_ = hr.cookies_;
        return *this;
    }

    static HttpRequest parse(const void* raw, size_t len);

    static HttpRequest parse(const std::string& raw) {
        return parse(raw.c_str(), raw.size());
    }

    HTTP::ReqMethod getMethod(void) const noexcept {
        return request_.getMethod();
    };

    HTTP::URI getURI(void) const {
        return request_.getURI();
    }

    HTTP::ProtocolVersion getVersion(void) const {
        return request_.getVersion();
    }

    Buffer getBuffer(void) const;
    
    std::string getString(void) const;

    HTTP::RequestLine& getRequestLine(void) noexcept {
        return request_;
    }

    HTTP::RequestLine getcRequestLine(void) {
        return request_;
    }

    HTTP::Headers& headers() {
        return headers_;
    }

    HTTP::Cookies& cookies() {
        return cookies_;
    }

    std::vector<char>& body(void) {
        return body_;
    }

    HttpRequest& setMethod(HTTP::ReqMethod method) {
        request_.setMethod(method);
        return *this;
    }

    HttpRequest& addHeader(const HTTP::Header& header) noexcept {
        headers_.addHeader(header);
        return *this;
    }

    HttpRequest& addHeader(HTTP::HeaderFieldName key, 
                           const std::string& value) noexcept {
        headers_.addHeader(key, value);
        return *this;
    }

    HttpRequest& setURI(const HTTP::URI& uri) noexcept {
        request_.setURI(uri);
        return *this;
    }

    HttpRequest& setURI(HTTP::URI&& uri) noexcept {
        request_.setURI(std::move(uri));
        return *this;
    }

    HttpRequest& setVersion(const HTTP::ProtocolVersion& ver) noexcept {
        request_.setVersion(ver);
        return *this;
    }

    HttpRequest& setVersion(HTTP::ProtocolVersion&& ver) noexcept {
        request_.setVersion(std::move(ver));
        return *this;
    }

protected:
    HTTP::RequestLine request_;
    HTTP::Headers     headers_;
    HTTP::Cookies     cookies_;
    std::vector<char> body_;

}; // class HttpRequest

} // namespace tab

#endif // __HTTP_REQUEST_HPP__