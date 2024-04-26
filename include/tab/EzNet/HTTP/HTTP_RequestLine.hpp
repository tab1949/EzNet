#ifndef __HTTP_REQUEST_LINE_HPP__
#define __HTTP_REQUEST_LINE_HPP__

#include <string>
#include <cstring>
#include <utility>

#include "EzNet/HTTP/HTTP_Protocol.hpp"
#include "EzNet/Utility/Network/URL.hpp"

namespace tab {

namespace HTTP {

enum ReqMethod {
    REQ_NONE = 0,
    /* Since HTTP 1.0 */
    REQ_GET, 
    REQ_HEAD, 
    REQ_POST, 
    /* Since HTTP 1.1 */
    REQ_PUT, 
    REQ_DELETE, 
    REQ_CONNECT, 
    REQ_OPTIONS, 
    REQ_TRACE, 
    REQ_PATCH
};

extern const char* ReqName[];

class RequestLine {
public:
    RequestLine(ReqMethod met, const URI& uri, 
                const ProtocolVersion& ver = "1.1") :
        method_(met), 
        uri_(uri),
        version_(ver),
        empty_(false) { }

    RequestLine(const RequestLine& req) :
        method_(req.method_),
        uri_(req.uri_),
        version_(req.version_),
        empty_(false) { }

    RequestLine(RequestLine&& req) noexcept :
        method_(req.method_), 
        uri_(std::move(req.uri_)), 
        version_(std::move(req.version_)),
        empty_(req.empty_) { }

    RequestLine(void) : empty_(true) { }

    static RequestLine parse(const std::string& raw);

    RequestLine& swap(RequestLine& req) noexcept {
        std::swap(empty_, req.empty_);
        std::swap(method_,req.method_);
        uri_.swap(req.uri_);
        version_.swap(req.version_);
        return *this;
    }

    bool empty(void) const noexcept {
        return empty_;
    }

    void clear(void) noexcept {
        empty_ = true;
        method_ = REQ_NONE;
        uri_.clear();
        version_.clear();
    }

    size_t size(void) const {
        // Why +7? : " " after method & " HTTP/" (Total: 7 bytes)
        return std::strlen(ReqName[method_]) + uri_.size() + version_.size() + 7;
    }

    RequestLine& operator=(const RequestLine& req) {
        reset(req);
        return *this;
    }

    RequestLine& operator=(RequestLine&& req) noexcept {
        return swap(req);
    }

    bool operator==(const RequestLine& req) noexcept {
        return (
            empty_  == req.empty_ &&
            method_ == req.method_ &&
            uri_    == req.uri_ &&
            version_== req.version_
        );
    }

    ReqMethod getMethod(void) const noexcept {
        return method_;
    }

    std::string getMethodName(void) const {
        return ReqName[method_];
    }

    URI getURI(void) const {
        return uri_;
    }

    ProtocolVersion getVersion(void) const {
        return version_;
    }

    std::string get(void) const {
        std::string ret(ReqName[method_]);
        ret.push_back(' ');
        ret.append(uri_);
        ret.append(" HTTP/");
        ret.append(version_);
        ret.append("\r\n");
        return ret;
    }

    std::string getRequestLine(void) const noexcept {
        return get();
    }

    std::string getStr(void) const noexcept {
        return get();
    }

    operator std::string(void) const {
        return std::move(get());
    }

    void reset(const RequestLine& req) noexcept {
        empty_  = req.empty_;
        if (empty_)
            return;
        method_ = req.method_;
        uri_    = req.uri_;
        version_= req.version_;
    }

    void reset(RequestLine&& req) noexcept {
        empty_  = req.empty_;
        if (empty_)
            return;
        method_ = req.method_;
        uri_    = std::move(req.uri_);
        version_= std::move(req.version_);
    }

    bool set(ReqMethod method, const URI& uri, const ProtocolVersion& ver) {
        method_ = method;
        uri_    = uri;
        version_= ver;
        empty_  = false;
        return true;
    }

    void setMethod(ReqMethod method) noexcept {
        method_ = method;
        empty_ = false;
    }

    void setURI(const URI& uri) noexcept {
        uri_ = uri;
        empty_ = false;
    }
    
    void setURI(URI&& uri) noexcept {
        uri_ = std::move(uri);
        empty_ = false;
    }

    void setVersion(const ProtocolVersion& ver) noexcept {
        version_ = ver;
    }

    void setVersion(ProtocolVersion&& ver) noexcept {
        version_ = std::move(ver);
    }
    
protected:
    ReqMethod       method_;
    URI             uri_;
    ProtocolVersion version_;
    bool            empty_;
    
}; // class RequestLine

} // namespace HTTP

} // namespace tab

#endif // __HTTP_REQUEST_LINE_HPP__