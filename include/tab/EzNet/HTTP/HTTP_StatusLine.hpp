#ifndef __HTTP_STATUS_LINE_HPP__
#define __HTTP_STATUS_LINE_HPP__

#include <string>
#include <utility>

#include "EzNet/HTTP/HTTP_Protocol.hpp"

namespace tab {

namespace HTTP {

enum class RespCode {
    /* 1xx - Informational */
    CONTINUE = 100,
    SWITCHING_PROTOCOLS = 101,
    /* 2xx - Success */
    OK       = 200,
    CREATED  = 201,
    ACCEPTED = 202,
    NON_AUTHORITATIVE_INFORMATION = 203,
    NO_CONTENT         = 204,
    RESET_CONTENT      = 205,
    PARTIAL_CONTENT   = 206,
    /* 3xx - Redirection */
    MULTIPLE_CHOICES   = 300,
    MOVED_PERMANENTLY  = 301,
    FOUND              = 302,
    SEE_OTHER          = 303,
    NOT_MODIFIED       = 304,
    USE_PROXY          = 305,
    UNUSED             = 306,
    TEMPORARY_REDIRECT = 307,
    /* 4xx - Client error */
    BAD_REQUEST        = 400,
    UNAUTHORIZED       = 401,
    PAYMENT_REQUIRED   = 402,
    FORBIDDEN	       = 403,
    NOT_FOUND	       = 404,
   	METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE	   = 406,
    PROXY_AUTHENTICATION_REQUIRED = 407,
    REQUEST_TIMEOUT   = 408,
    CONFLICT          = 409,
    GONE	          = 410,
    LENGTH_REQUIRED	  = 411,
    PRECONDITION_FAILED	     = 412,
    REQUEST_ENTITY_TOO_LARGE = 413,
    REQUEST_URI_TOO_LARGE	 = 414,
    UNSUPPORTED_MEDIA_TYPE   = 415,
    REQUESTED_RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,
    /* 5xx - Server error */
    INTERNAL_SERVER_ERROR	= 500,
    NOT_IMPLEMENTED	    = 501,
    BAD_GATEWAY	        = 502,
    SERVICE_UNAVAILABLE	= 503,
    GATEWAY_TIMEOUT	    = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505
};

class StatusLine {
public:
    StatusLine(const StatusLine& sl) = default;

    StatusLine(StatusLine&& sl) noexcept : 
        version_(std::move(sl.version_)),
        code_(std::move(sl.code_)),
        phrase_(std::move(sl.phrase_)) {
    }

    StatusLine(void) : version_("2.0"), code_(RespCode::OK), phrase_("OK") { }

    StatusLine(const ProtocolVersion& ver, RespCode code,
               const ResPhrase& phrase) :
        version_(ver),
        code_(code),
        phrase_(phrase) {
    }

    StatusLine(const std::string& line) : 
        StatusLine(std::move(parse(line))) {
    }
    
    
    static StatusLine parse(const char* line, size_t len);

    static StatusLine parse(const std::string& line) {
        return std::move(parse(line.c_str(), line.size()));
    }

    StatusLine& operator=(const StatusLine& val) {
        version_ = val.version_;
        code_    = val.code_;
        phrase_  = val.phrase_;
        return *this;
    }

    StatusLine& operator=(StatusLine&& val) {
        version_ = std::move(val.version_);
        code_    = val.code_;
        phrase_  = std::move(val.phrase_);
        return *this;
    }

    bool operator==(const StatusLine& val) const noexcept {
        return (
            version_ == val.version_ &&
            code_ == val.code_ &&
            phrase_ == val.phrase_
        );
    }

    std::string getStr(void) const {
        std::string ret;
        ret.append("HTTP/");
        ret.append(version_);
        ret.push_back(' ');
        ret.append(std::to_string((int)code_));
        ret.push_back(' ');
        ret.append(phrase_);
        ret.append("\r\n");
        return ret;
    }

    operator std::string() const {
        return getStr();
    }

    ProtocolVersion getVersion(void) const noexcept {
        return version_;
    }

    RespCode getCode(void) const noexcept {
        return code_;
    }

    ResPhrase getPhrase(void) const noexcept {
        return phrase_;
    }

    void setVersion(const ProtocolVersion& ver) {
        version_ = ver;
    }

    void setVersion(ProtocolVersion&& ver) noexcept {
        version_ = std::move(ver);
    }

    void setCode(RespCode code) noexcept {
        code_ = code;
    }

    void setPhrase(const ResPhrase& phrase) {
        phrase_ = phrase;
    }

    void setPhrase(ResPhrase&& phrase) noexcept {
        phrase_ = std::move(phrase);
    }

private:
    ProtocolVersion version_;
    RespCode code_;
    ResPhrase phrase_;
    
}; // class StatusLine

} // namespace HTTP

} // namespace tab

#endif // __HTTP_STATUS_LINE_HPP__