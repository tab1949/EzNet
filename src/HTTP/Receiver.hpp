#ifndef __RECEIVER_HPP__
#define __RECEIVER_HPP__

#include <functional>
#include <memory>

#include "EzNet/Socket/Socket.hpp"
#include "EzNet/HTTP/HTTP_Response.hpp"
#include "EzNet/Utility/IO/IO.hpp"

namespace tab {

class Receiver {
public:
    Receiver() = delete;
    Receiver(void* buf, size_t len, 
        const std::function<size_t(void*,size_t)>& w) : 
            buffer_((char*)buf), buffer_length_(len), write_(w) {}

    Receiver& receive(Socket*);

    char* buffer_;
    size_t buffer_length_;
    
    HttpResponse resp_;
    std::function<size_t(void*,size_t)> write_;

}; // class Receiver

} // namespace tab

#endif // __RECEIVER_HPP__