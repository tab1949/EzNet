#include <climits>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <exception>
#include <utility>

#include "EzNet/Socket/StreamSocket.hpp"
#include "EzNet/HTTP/HTTP_Session.hpp"
#include "EzNet/Utility/General/Transform.hpp"

#include "Receiver.hpp"

namespace tab {
    
void HttpSession::close() {
    alive_ = false;
    socket_.reset();
}

HttpSessionClient::HttpSessionClient(SocketGenerator sg) : 
    HttpSession(new HttpRequest(HTTP::REQ_GET, "/"), 
                new HttpResponse),
    writer_(StdWriter()),
    get_socket_(sg) { }


HttpSessionClient::HttpSessionClient(const HttpSessionClient& hs) :
    HttpSession(new HttpRequest(*hs.request_), 
                new HttpResponse(*hs.response_)),
    target_(hs.target_),
    writer_(hs.writer_),
    get_socket_(hs.get_socket_) { }
    

HttpSessionClient::HttpSessionClient(HttpSessionClient&& hs) :
    HttpSession(std::move(hs.request_), std::move(hs.response_)),
    target_(std::move(hs.target_)),
    writer_(std::move(hs.writer_)),
    get_socket_(std::move(hs.get_socket_)) {
        std::swap(socket_, hs.socket_);
        std::swap(alive_, hs.alive_);
    }


HttpSessionClient& HttpSessionClient::operator=(HttpSessionClient&& hs) {
    request_ = std::move(hs.request_);
    response_ = std::move(hs.response_);
    get_socket_ = std::move(hs.get_socket_);
    target_ = std::move(hs.target_);
    writer_ = std::move(hs.writer_);
    std::swap(socket_, hs.socket_);
    std::swap(alive_, hs.alive_);
    return *this;
}


HttpSessionClient& HttpSessionClient::operator=(const HttpSessionClient& hs) {
    *request_ = *hs.request_;
    *response_ = *hs.response_;
    get_socket_ = hs.get_socket_;
    target_ = hs.target_;
    writer_ = hs.writer_;
    return *this;
}


HttpSessionClient& HttpSessionClient::setURL(const URL& url) {
    if (url.getProtocol() != URL::Protocol::HTTP && 
        url.getProtocol() != URL::Protocol::HTTPS) {
        throw std::logic_error(
            "tab::HttpSessionClient::setURL(): "
            "The protocol is not HTTP or HTTPS.");
    }
    request_->setURI(url.getPath());
    request_->addHeader(HTTP::HOST, url.getHostName());
    target_ = url;
    if (alive_) {
        socket_->shutdown();
        socket_->close();
        alive_ = false;
    }
    return *this;
}


HttpSessionClient& HttpSessionClient::setKeepAlive(bool opt) {
    options_.keep_alive = opt;
    if (opt)
        request_->addHeader(HTTP::CONNECTION, "keep-alive");
    else
        request_->headers().remove(HTTP::CONNECTION);
    return *this;
}


void HttpSessionClient::performSingleRequest(
    char* recv_buffer, const size_t recv_buffer_size) {
    if (!alive_) {
        socket_ = get_socket_(
            target_.getHost().getAddr().getAF(), 
            target_.getProtocol() == URL::Protocol::HTTPS ? (char)1 : (char)0);
        try{
            if (!socket_->connect(
                target_.getHost().getAddr().setProtocol(IPPROTO_TCP))) {
                throw std::runtime_error(
                    "tab::HttpSessionClient::request(): "
                    "Can not connect to the destination.");
            }
        }
        catch (const std::exception& e) {
            throw std::runtime_error(
                std::string(
                    "tab::HttpSessionClient::request(): "
                    "Error(s) occurred while connecting "
                    "to the destination.\r\n  std::exception::what(): "
                ) + e.what()
            );
        }
        alive_ = true;
    }

    auto request_buffer = request_->getBuffer();

    size_t bytes_sent = 0;
    try {
        bytes_sent = socket_->send(
            request_buffer.begin(), static_cast<int>(request_buffer.size()));
        if(bytes_sent != request_buffer.size())
            throw std::runtime_error(
                std::string("Bytes sent is not equal to the request data size (sent=") +
                std::to_string(bytes_sent) + ", data=" +
                std::to_string(request_buffer.size()));
    } catch (const std::exception& e) {
        alive_ = false;
        throw std::runtime_error (
                std::string(
                    "tab::HttpSessionClient::request(): Error(s) occurred "
                    "while sending request to the destination.\r\n  "
                    "std::exception::what(): "
                ) + e.what()
            );
    }
    request_buffer.release();
        
    Receiver receiver(recv_buffer, recv_buffer_size, writer_);
    receiver.receive(socket_.get());
    *response_ = std::move(receiver.resp_);
        
    if (!options_.keep_alive || 
        response_->getHeaders().find(HTTP::CONNECTION) == "close") {
        alive_ = false;
        socket_.reset(); // equals to the code below
        // socket_->shutdown();
        // socket_->close();
    }
}


void HttpSessionClient::performSerialRequest() {
    const size_t recv_buffer_size = 0x1000;
    std::unique_ptr<char[]> recv_buffer(new char[recv_buffer_size]);

    bool keep_on_perform = false;
    
    do {
    performSingleRequest(recv_buffer.get(), recv_buffer_size);

    switch((int)response_->getCode()) {
        case 301: // fallthrough
        case 302: {
            std::string&& sLocation(
                response_->getHeaders().find(HTTP::LOCATION));
            if (sLocation.empty()) // "Location" is not in the response headers
                break;
                
            URL location(sLocation);
            // No location specified, 
            // or the host is same as current one (IP address is not changed)
            if (location.getHostName().empty() || 
                location.getHost() == target_.getHost()) {
                request_->setURI(sLocation);
            }
            else {
                target_ = location;
                if (alive_) 
                    alive_ = false;
                request_->addHeader(HTTP::HOST, location.getHostName());
            }

            keep_on_perform = true;
            break;
        } // case 302

        default: {
            keep_on_perform = false; 
            break;
        }
    } // switch()

    } while (keep_on_perform); // Exit when there is no task to do.

}


HttpResponse& HttpSessionClient::request() {
    if (options_.auto_jump) {
        performSerialRequest();
    }
    else {
        const size_t recv_buffer_size = 0x1000;
        std::unique_ptr<char[]> recv_buffer(new char[recv_buffer_size]);
        performSingleRequest(recv_buffer.get(), recv_buffer_size);
    }
    return getResponse();
} // HttpSessionClient::request()


} // namespace tab