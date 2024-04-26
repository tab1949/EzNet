#include <algorithm>

#include "EzNet/HTTP/HTTP_Request.hpp"

namespace tab {

namespace HTTP {

const char* ReqName[]{
    "",
    /* Since HTTP 1.0 */
    "GET",
    "HEAD",
    "POST",
    /* Since HTTP 1.1 */
    "PUT",
    "DELETE",
    "CONNECT",
    "OPTIONS",
    "TRACE",
    "PATCH"
};

RequestLine RequestLine::parse(const std::string& raw) {
    size_t i = 0, j = 0;
    size_t limit = raw.size();
    RequestLine ret;
    
    // Get the method.
    for (; i < limit; ++i) {
        if (raw[i] == ' ') {
            std::string&& method = std::string(raw.begin(), raw.begin() + i);
            const char** pos
                = find(ReqName, 
                       ReqName + sizeof(ReqName) / sizeof(const char*),
                       method);
            if (pos != ReqName + sizeof(ReqName) / sizeof(const char*))
                ret.method_ = ReqMethod(pos - ReqName);
            break;
        }
    }

    for (; i < limit; ++i)
        if (raw[i] != ' ')
            break;
    for (j = limit; j >= i; --j)
        if (raw[j] == ' ')
            break;
    ret.uri_ = std::move(std::string(raw.begin() + i, raw.begin() + j));

    for (i = j; i < limit; ++i) {
        if (raw[i] == '/') {
            ++i;
            break;
        }
    }
    for (j = i; j < limit; ++j)
        if (raw[j] == '\r')
            break;
    ret.version_ = std::move(std::string(raw.begin() + i, raw.begin() + j));

    ret.empty_ = false;

    return ret;
}

} // namespace HTTP



/**
 * A standard HTTP request should be like:
 * 
 * 
 * GET /index.html HTTP/1.1
 * User-Agent: XXX/0.0
 * 
 * 
 * or
 * 
 * 
 * POST / HTTP/1.1
 * User-Agent: XXXX/0.0.0 
 * content
 * 
 * .
 * 
 * This function parses the requests by finding the '\r' at the end of lines, 
 * and it will not throw exceptions on its own.
 * Lines will be resolved by other functions 
 * which are likely to throw exceptions.
 * 
 * Most syntax errors will cause an exception throwing,
 * but if it is too covert,
 * the questionable content will be seen as normal. 
 * This problem should be noticed by users.
 */
HttpRequest HttpRequest::parse(const void* raw, size_t len) {
    size_t i = 0, j = 0;
    auto content_ptr = static_cast<const char*>(raw);
    
    
    for (; i < len; ++i)
        if (content_ptr[i] == '\r')
            break;
    HttpRequest ret(std::move(
        HTTP::RequestLine::parse(std::string(content_ptr, content_ptr + i))));

    for (i += 2, j = i; j < len; ++j) {
        if (content_ptr[j] == '\r') {
            std::string&& line = std::string(content_ptr + i, content_ptr + j);
            i = j + 2;
            if (!line.empty()) {
                ret.addHeader(HTTP::Header::parse(line));
                ++j;
            }
            else {
                break;
            }
        }
    }

    if (ret.getMethod() == HTTP::REQ_POST) {
        ret.body_.assign(content_ptr + i, content_ptr + len);
    }
    
    return ret;
}

std::string HttpRequest::getString(void) const {
    std::string ret;
    ret.append(request_.get());
    ret.append(headers_.getStr());
    if (cookies_.size() > 0)
        ret.append(cookies_.getUploadString());
    ret.append("\r\n");
    ret.append(body_.begin(), body_.end());
    return ret;
}

Buffer HttpRequest::getBuffer(void) const {
    std::string str;
    str.append(request_.get());
    str.append(headers_.getStr());
    if (cookies_.size() > 0)
        str.append(cookies_.getUploadString());
    str.append("\r\n");

    Buffer ret(str.size() + body_.size());
    ret.append(str.begin(), str.end());
    ret.append(body_.begin(), body_.end());
    
    return ret;
}

} // namespace tab