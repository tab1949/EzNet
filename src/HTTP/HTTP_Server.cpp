#include "EzNet/HTTP/HTTP_Server.hpp"
#include "EzNet/Utility/General/Transform.hpp"
#include <iostream>
namespace tab {

HttpServer::HttpServer(const TcpConfig& c) : TcpServer(c) {
    loadEventListeners();
}

HttpServer::HttpServer() : TcpServer() {
    loadEventListeners();
}

void HttpServer::loadEventListeners() {
    auto matcher_ptr = &event_matcher_;

    registerEvent<DataReceivedEvent>([matcher_ptr](DataReceivedEvent& e) {
        if (e.getContentSize() == e.getBufferSize()) {
            // TODO: e.requireAdditionalBuffer()
        }
        
        auto&& req = HttpRequest::parse(e.getBuffer(), e.getBufferSize());

        bool keep_alive = false;
        try {
            auto&& str = req.headers().find(HTTP::CONNECTION);
            if (!str.empty()) {
                if (UppercaseToLower(str) == "keep-alive") {
                    keep_alive = true;
                    e.flag() = TcpServerEvent::OP_READ;
                }
                else if (str == "close") {
                    e.flag() = TcpServerEvent::OP_CLOSE;
                }
                else
                    throw (char)0;
            }
            else
                throw (char)0;
        }
        catch (char) { // header "Connection" is not found or the value is invalid
            if (req.getRequestLine().getVersion() == "1.0") {
                e.flag() = TcpServerEvent::OP_CLOSE;
            }
            else {
                keep_alive = true;
                e.flag() = TcpServerEvent::OP_READ;
            }
        }

        HttpRequestReceivedEvent event(std::move(req));
        matcher_ptr->call(event);
        
        if (event.close_ || !keep_alive) 
            e.flag() = TcpServerEvent::OP_CLOSE;
        event.response_
            .getHeaders()
            .addHeader(HTTP::CONTENT_LENGTH, 
                std::to_string(event.response_.getBody().size()).c_str());
        auto&& response = event.response_.getStr();
        memcpy(e.getBuffer(), response.c_str(), response.size());
        e.setContentSize(static_cast<unsigned long>(response.size()));
        e.setNextOperation(TcpServerEvent::OP_WRITE);
    });  // DataReceivedEvent

    // This handler will be replaced if the user registers later.
    registerEvent<HttpRequestReceivedEvent>([](HttpRequestReceivedEvent){});

    registerEvent<DataSentEvent>([](DataSentEvent& e) {
        if (e.flag() == TcpServerEvent::OP_CLOSE) {
            e.setNextOperation(TcpServerEvent::OP_CLOSE);
        }
        else {
            // TODO: Determine which buffer to use
            e.setNextOperation(TcpServerEvent::OP_READ);
        }
    });
}


} // namespace tab