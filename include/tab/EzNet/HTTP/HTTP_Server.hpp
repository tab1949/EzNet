#ifndef __HTTP_SERVER_HPP__
#define __HTTP_SERVER_HPP__

#include "EzNet/Socket/TcpServer.hpp"

#include "HTTP_Request.hpp"
#include "HTTP_Response.hpp"

namespace tab {

class HttpServer : public TcpServer {
public:
    struct HttpConfig {
        bool keep_alive = false;
    };

public:
    HttpServer();
    HttpServer(const TcpConfig&);

    HttpConfig& configHTTP() {
        return config_http_;
    }

private:
    void loadEventListeners();

    HttpConfig config_http_;

}; // class HttpServer


class HttpServerEvent : public TcpServerEvent {
public:
    constexpr static event_type_id_t GetEventTypeID() {
        return (1 << 7) + (1  << 6);
    }

}; // class HttpServerEvent


class HttpRequestReceivedEvent : public HttpServerEvent {
public:
    constexpr static event_type_id_t GetEventTypeID() {
        return HttpServerEvent::GetEventTypeID() + 1;
    }

    auto& getRequest() {
        return request_;
    }

    auto& getResponse() {
        return response_;
    }

    void close() {
        close_ = true;
    }

protected:
    HttpRequest request_;
    HttpResponse response_;
    bool close_ = false;

    HttpRequestReceivedEvent(HttpRequest&& r) : request_(std::move(r)) { }

    friend class HttpServer;
}; // class HttpServerReceivedEvent

} // namespace tab

#endif // __HTTP_SERVER_HPP__