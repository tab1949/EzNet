#include <algorithm>
#include <exception>
#include <functional>
#include <utility>

#include "Receiver.hpp"

#include "EzNet/Utility/Memory/Memory.hpp"
#include "EzNet/Utility/General/Transform.hpp"

namespace tab {

class ReceiveQueue {
public:
    ReceiveQueue(Socket* s, void* buffer, size_t length) : 
        sock_(s), buf_((char*)buffer), buf_len_(length), recv_(0), cur_(0) { }

    void read(std::function<void(void*, size_t)> w, size_t len) {
        if (cur_ < recv_) {
            auto temp = recv_ - cur_;
            if (len <= temp) {
                w(buf_ + cur_, len);
                cur_ += len;
                return;
            }
            else {
                w(buf_ + cur_, temp);
                len -= temp;
                cur_ = recv_;
            }
        }
        for (;;) {
            if (len == 0) 
                break;
            recv_ = sock_->recv(buf_, static_cast<int>(buf_len_));
            cur_ = 0;
            if (recv_ <= 0)
                throw recv_;
            if (len >= recv_) {
                w(buf_, recv_);
                len -= recv_;
                cur_ = recv_;
            } 
            else {
                w(buf_, len);
                cur_ = len;
                return;
            }
        }
    }

    void read(void* des, size_t len) {
        char* ptr = (char*)des;
        read([&ptr](void* s, size_t l) {
            std::copy((char*)s, (char*)s + l, ptr); 
            ptr += l;
        }, len);
    }

    char read() {
        if (cur_ >= recv_) {
            recv_ = sock_->recv(buf_, static_cast<int>(buf_len_));
            if (recv_ <= 0)
                throw recv_;
            cur_ = 0;
        }
        ++ cur_;
        return buf_[cur_ - 1];
    }

    size_t waiting_for_reading() {
        return recv_ - cur_;
    }

    size_t begin() {
        return cur_;
    }

private:
    Socket* sock_;
    char* buf_;
    size_t buf_len_;
    size_t recv_;
    size_t cur_;
};

// TODO: Complete this method 
Receiver& Receiver::receive(Socket* s) {
    Buffer temp(4096);
    ReceiveQueue receive_queue(s, buffer_, buffer_length_);

    bool has_cr = false;
    for (size_t n_line = 0;;) {
        char cur = 0;
        
        try {
            cur = receive_queue.read();
        }
        catch (int) {
            return *this;
        }

        if (cur == '\r') {
            has_cr = true;
        }
        else if (cur == '\n' && has_cr) {
            has_cr = false;
            if (temp.size() == 0) {
                break;
            }
            if (n_line == 0)
                resp_.status_line_ = std::move(
                    HTTP::StatusLine::parse(temp.begin(), temp.size()));
            else {
                auto&& header = HTTP::Header::parse(temp.begin(), temp.size());
                if (header.type() == HTTP::Header::Type::Common) {
                    auto& converted_header = header.to<HTTP::CommonHeader>();
                    if (converted_header.getKey() == HTTP::SET_COOKIE)
                        resp_.getCookies()
                             .add(HTTP::Cookie::parse(
                                converted_header.getValue()));
                    else
                        resp_.addHeader(std::move(header));
                }
                else {
                    std::string key_str(std::move(header.getKeyString()));
                    if (UppercaseToLower(key_str) == "set-cookie")
                        resp_.getCookies()
                             .add(HTTP::Cookie::parse(header.getValue()));
                    else
                        resp_.addHeader(std::move(header));
                }
            }
            ++n_line;
            temp.clear();
        }
        else {
            temp.append(cur);
        }
    }

    std::string transfer_encoding(
        std::move(resp_.getHeaders().find(HTTP::TRANSFER_ENCODING)));

    if (transfer_encoding.empty()) {
        auto&& content_length = resp_.headers_.find(HTTP::CONTENT_LENGTH);
        if (!content_length.empty()) {
            receive_queue.read(write_, std::stoull(content_length));
        }
        else {
            write_(buffer_ + receive_queue.begin(), 
                   receive_queue.waiting_for_reading());
            for (int n;;) {
                n = s->recv(buffer_, static_cast<int>(buffer_length_));
                if (n <= 0)
                    break;
                write_(buffer_, n);
            }
        }
    }
    else if (transfer_encoding.find("chunked") != std::string::npos) {
        temp.release();
        std::string chunk_len_str;
        size_t chunk_len;

        char recv_char = receive_queue.read();
        for (;;) {
            for (; ; recv_char = receive_queue.read()) {
                if (recv_char != '\r') {
                    if (recv_char != ' ') {
                        chunk_len_str.push_back(recv_char);
                    }
                    else {
                        for (; 
                        recv_char == ' '; 
                        recv_char = receive_queue.read());
                        break;
                    }
                }
                else
                    break;
            }

            if (recv_char == '\r')
                recv_char = receive_queue.read();
            if (recv_char == '\n')
                recv_char = receive_queue.read();

            chunk_len = HexStrToULL(chunk_len_str);
            chunk_len_str.clear();
            if (chunk_len == 0) {
                for (; recv_char == '\r'; recv_char = receive_queue.read());
                break;
            }
            write_(&recv_char, 1);
            -- chunk_len;
            receive_queue.read(write_, chunk_len);
            recv_char = receive_queue.read();

            if (recv_char == '\r')
                recv_char = receive_queue.read();
            if (recv_char == '\n')
                recv_char = receive_queue.read();
        }
    }

    return *this;
}

} // namespace tab