/**
 * @file HTTP_Session.hpp
 * @author 2969117392@qq.com
 * @brief Some declarations and definitions of class 
 * @brief 'tab::HttpSession', 'tab::HttpSessionClient' 
 * @brief and 'tab::HttpSessionServer'.
 * 
 * @note For more information, please view the comments of each class.
 * 
 * @date 2023-09-29
 * 
 */
#ifndef __HTTP_SESSION_HPP__
#define __HTTP_SESSION_HPP__

#include <functional>
#include <memory>

#include "EzNet/Socket/StreamSocket.hpp"
#include "EzNet/Utility/Network/URL.hpp"
#include "EzNet/Utility/IO/IO.hpp"
#include "HTTP_Request.hpp"
#include "HTTP_Response.hpp"

namespace tab {

/**
 * @brief The parent of 'HttpSessionClient' and 'HttpSessionServer',
 * @brief which has the members they both need.
 * 
 */
class HttpSession {
public:
    void close();

    HttpSession& swap(HttpSession& val) {
        std::swap(request_, val.request_);
        std::swap(response_, val.response_);
        std::swap(socket_, val.socket_);
        std::swap(alive_, val.alive_);
        return *this;
    }
protected:
    /**
     * @brief This protected constructor is designed for 
     * @brief the subclasses of this class.
     *
     * @param a Object of 'HttpRequest'
     * @param b Object of 'HttpResponse'
     */
    template<typename T1, typename T2>
    HttpSession(T1&& a, T2&& b) : request_(std::move(a)), response_(std::move(b)) {
        
    }

    // To keep the requesting information.
    std::unique_ptr<HttpRequest> request_;
    // To keep the response information.
    std::unique_ptr<HttpResponse> response_;

    std::shared_ptr<Socket> socket_;
    // Indicates whether the socket is able to read or write.
    // This variable will be 'false' when 'socket_' is null or closed or has some problems.
    bool alive_ = false; 

}; // class HttpSession


/**
 * @brief A kind of 'HttpSession' for client use.
 * @brief It can be used to make a HTTP request.
 * 
 */
class HttpSessionClient : public HttpSession {
protected:
    using SocketGenerator = std::function<std::shared_ptr<Socket>(af_t, char)>;
    
    // For 'GetNewSession()' in HTTP_Client.cpp
    HttpSessionClient(SocketGenerator sg);
    friend HttpSessionClient GetNewSession();

public:
    // Copy
    HttpSessionClient(const HttpSessionClient& hs);
        
    // Move
    HttpSessionClient(HttpSessionClient&& hs);

    // This destructor won't do anything else, because
    // all the members can be destructed safely without
    // any other operations. (Thanks to smart pointers.)
    ~HttpSessionClient() {
    }

    // Move (or swap)
    HttpSessionClient& operator=(HttpSessionClient&& hs);

    // Copy
    HttpSessionClient& operator=(const HttpSessionClient& hs);

    /**
     * @brief Update current target URL.
     * 
     * @param url URL
     * @return HttpSessionClient& - this object
     * 
     * @warning It will do some inspections to make sure that the provided URL is legal. 
     * @warning Exceptions (std::logic_error) will be thrown when the protocol is not HTTP or HTTPS.
     * 
     * @note If you are only a user, this method is not necessary to be called after you got
     * @note an object of this class. But developers *M*U*S*T* pay attention that URL should be 
     * @note set after construction.
     */
    HttpSessionClient& setURL(const URL& url);

    /**
     * @brief Update current target URI. Other components in
     * @brief the URL will not be changed.
     * 
     * @param uri 
     * @return HttpSessionClient& 
     */
    HttpSessionClient& setURI(const HTTP::URI& uri) {
        request_->setURI(uri);
        return *this;
    }

    /**
     * @brief Add a new HTTP header to the next request.
     * 
     * @param key Key
     * @param value Value
     * @return HttpSessionClient& This object 
     */
    HttpSessionClient& addHeader(HTTP::HeaderFieldName key, const std::string& value) {
        request_->addHeader(key, value);
        return *this;
    }

    /**
     * @brief Set the method of the next request.
     * 
     * @param method HTTP request method
     * @return HttpSessionClient& This object
     */
    HttpSessionClient& setMethod(HTTP::ReqMethod method) {
        request_->setMethod(method);
        return *this;
    }

    /**
     * @brief Set the writer for this session.
     * 
     * @param writer 
     * @return HttpSessionClient& This object
     *
     * @note Data will be written into the default stream(std::cout in most cases).
     * @note You'd better use this method to change way to write data.
     */
    template <class _Writer>
    HttpSessionClient& setWriter(_Writer&& writer) {
        writer_.operator=(writer);
        return *this;
    }

    HttpSessionClient& setKeepAlive(bool opt = true);

    /**
     * @brief The target host(or protocol, URI, port, etc.) will be changed automatically
     * @brief if you enabled "Auto Jump"(set the parameter to 'true').
     * 
     * @param opt Enable(default) or disable
     * 
     * @return HttpSessionClient& This object
     */
    HttpSessionClient& setAutoJump(bool opt = true) {
        options_.auto_jump = opt;
        return *this;
    }

    /**
     * @brief Get the reference to the request data.
     */
    HttpRequest& getRequest(void) {
        return *request_;
    }

    /**
     * @brief Get the reference to the response data.
     */
    HttpResponse& getResponse(void) {
        return *response_;
    }

    /**
     * @brief Perform a request.
     * 
     * @return The response data.
     */
    HttpResponse& request(void);

protected:
    // send a request and receive a response
    void performSingleRequest(char*, const size_t);
    // send and receive till the response code is not 301 or 302
    void performSerialRequest();

protected:
    struct {
        bool allow_cookies = false;
        bool auto_jump     = true;
        bool keep_alive    = true;
    } options_;
    URL target_;
    std::function<size_t(const void*, size_t)> writer_;
    SocketGenerator get_socket_;
    
}; // class Session


class HttpSessionServer : public HttpSession {
public:

}; // class HttpSessionServer


} // namespace tab

#endif // __HTTP_SESSION_HPP__