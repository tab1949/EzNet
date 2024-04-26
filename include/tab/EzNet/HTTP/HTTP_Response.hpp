#ifndef __HTTP_RESPONSE_HPP__
#define __HTTP_RESPONSE_HPP__

#include <string>

#include "EzNet/HTTP/HTTP_StatusLine.hpp"
#include "EzNet/HTTP/HTTP_Header.hpp"
#include "EzNet/HTTP/HTTP_Cookie.hpp"

namespace tab {

class HttpResponse {
public:
    HttpResponse(void) = default;

    HttpResponse(const HttpResponse&) = default;

    HttpResponse(HttpResponse&& val) noexcept:
        status_line_(std::move(val.status_line_)),
        headers_(std::move(val.headers_)),
        cookies_(std::move(val.cookies_)),
        body_(std::move(val.body_)) { }

    HttpResponse(const std::string& raw) :
        HttpResponse(std::move(parse(raw))) { }
    
    HttpResponse& operator=(const HttpResponse& val) {
        status_line_ = val.status_line_;
        headers_ = val.headers_;
        cookies_ = val.cookies_;
        body_ = val.body_;
        return *this;
    }
    
    HttpResponse& operator=(HttpResponse&& val) {
        status_line_ = std::move(val.status_line_);
        headers_ = std::move(val.headers_);
        cookies_ = std::move(val.cookies_);
        body_ = std::move(val.body_);
        return *this;
    }

    static HttpResponse parse(const std::string& raw);

    std::string getStr(void) const;

    operator std::string(void) const {
        return getStr();
    }

    void setStatusLine(const HTTP::StatusLine& sl) {
        status_line_ = sl;
    }

    void setStatusLine(HTTP::StatusLine&& sl) {
        status_line_ = std::move(sl);
    }

    void setHeaders(const HTTP::Headers& headers) {
        headers_ = headers;
    }

    void setHeaders(HTTP::Headers&& headers) {
        headers_ = std::move(headers);
    }

    void setCookies(const HTTP::Cookies& cookies) {
        cookies_ = cookies;
    }

    void setCookies(HTTP::Cookies&& cookies) {
        cookies_ = std::move(cookies);
    }

    void addHeader(const HTTP::Header& header) {
        headers_.addHeader(header);
    }

    void addHeader(HTTP::Header&& header) {
        headers_.addHeader(std::move(header));
    }

    HTTP::StatusLine& getStatusLine(void) noexcept {
        return status_line_;
    }

    HTTP::ProtocolVersion getVersion(void) const {
        return status_line_.getVersion();
    }

    HTTP::RespCode getCode(void) const noexcept {
        return status_line_.getCode();
    }

    HTTP::Headers& getHeaders(void) noexcept {
        return headers_;
    }

    HTTP::Cookies& getCookies() noexcept {
        return cookies_;
    }

    std::string& getBody(void) noexcept {
        return body_;
    }

protected:
    HTTP::StatusLine status_line_;
    HTTP::Headers headers_;
    HTTP::Cookies cookies_;
    std::string body_;

    friend class Receiver;
    
};

} // namespace tab

#endif // __HTTP_RESPONSE_HPP__