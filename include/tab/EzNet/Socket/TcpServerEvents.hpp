#ifndef __TCP_SERVER_EVENTS__
#define __TCP_SERVER_EVENTS__

#include <climits>
#include <exception>
#include <stdexcept>

#include "EzNet/Utility/Event/Event.hpp"

namespace tab {

class TcpServerEvent : public Event {
public:
    enum OPERATION {
        OP_CLOSE = 0,
        OP_READ = 1,
        OP_WRITE = 2
    };

    constexpr static event_type_id_t GetEventTypeID() {
        return 1 << 7;
    }

};  // class TcpServerEvent

class TcpServerEventBase : public TcpServerEvent {
public:
    char* getBuffer() const {
        return buffer_;
    }

    char* getExtendedBuffer() const {
        if (buffer_add_ == nullptr)
            throw std::runtime_error(
                "tab::DataReceivedEvent::getExtendedBuffer(): "
                "Extended buffer is not allocated.");
        return buffer_add_;
    }

    char* getActiveBuffer() const {
        if (active_buffer_ == EXTENDED) 
            return buffer_add_;
        else
            return buffer_;
    }

    unsigned long getBufferSize() const {
        return buffer_size_;
    }

    unsigned long getExtendedBufferSize() const {
        return buffer_add_size_;
    }

    unsigned long getContentSize() const {
        return content_size_;
    }

    unsigned long getExtendContentSize() const {
        return content_size_add_;
    }    
    
    unsigned long getActiveBufferSize() const {
        if (active_buffer_ == EXTENDED) 
            return buffer_add_size_;
        else
            return buffer_size_;
    }

    void setContentSize(unsigned long s) {
        content_size_ = s;
    }

    void setExtendedContentSize(unsigned long s) {
        content_size_add_ = s;
    }

    void setNextOperation(OPERATION o) {
        next_operation_ = o;
    }

    /**
     * @brief If the default buffer is not large enough,
     *        use this method to get an additional buffer.
     * 
     * @note Calling this function will lead to a memory allocation, 
     *       and the new buffer will be the buffer used for the next operation. 
     *       (This means that if you required a sending operation, 
     *       the server would send nothing. And if you required a 
     *       reading operation, the latest received data will be placed 
     *       in the new buffer.)
     * 
     * @param size The size of the additional buffer.  
     */
    char* extendBuffer(unsigned long size) {
        if (buffer_add_ != nullptr) {
            if (buffer_add_size_ <= size) return buffer_add_;
            delete[] buffer_add_;
        }
        // Do not care the deleting operation.
        // The buffer will be deleted automatically by 'SocketContext'.
        buffer_add_ = new char[size]();
        buffer_add_size_ = size;
        allocated_buffer_add_ = true;
        return buffer_add_;
    }

    enum BUFFER_CHOICE { DEFAULT, EXTENDED };
    char* setActiveBuffer(BUFFER_CHOICE which) { 
        active_buffer_ = which;
        if (which == DEFAULT) {
            return buffer_;
        } else {
            if (buffer_add_ == nullptr)
                throw std::runtime_error(
                    "tab::DataReceivedEvent::switchBuffer(): "
                    "Extended buffer is not allocated.");
            return buffer_add_;
        }
    }

    unsigned long long& flag() {
        return flag_;
    }

protected: 
    TcpServerEventBase(unsigned long long& f) : flag_(f) { }

private:
    char*         buffer_;
    unsigned long buffer_size_;
    unsigned long content_size_;
    char*         buffer_add_ = nullptr;
    unsigned long buffer_add_size_ = 0;
    unsigned long content_size_add_ = 0;
    bool          allocated_buffer_add_ = false;
    BUFFER_CHOICE active_buffer_ = DEFAULT;
    OPERATION     next_operation_ = OP_CLOSE;
    unsigned long long& flag_;

    friend class TcpEventInternal;

};

class ConnectionAcceptedEvent : public TcpServerEventBase {
public:
    constexpr static event_type_id_t GetEventTypeID() {
        return TcpServerEvent::GetEventTypeID() + 1;
    }

protected:
    ConnectionAcceptedEvent(unsigned long long& f) : TcpServerEventBase(f) { }

    friend class ConnectionAcceptedEventInternal;

};  // class ConnectionAcceptedEvent

class DataReceivedEvent : public TcpServerEventBase {
public:
    constexpr static event_type_id_t GetEventTypeID() {
        return TcpServerEvent::GetEventTypeID() + 2;
    }

protected:
    DataReceivedEvent(unsigned long long& f) : TcpServerEventBase(f) { }

    friend class DataReceivedEventInternal;
};  // class DataReceivedEvent

class DataSentEvent : public TcpServerEventBase {
public:
    constexpr static event_type_id_t GetEventTypeID() {
        return TcpServerEvent::GetEventTypeID() + 3;
    }

protected:
    DataSentEvent(unsigned long long& f) : TcpServerEventBase(f) { }

    friend class DataSentEventInternal;
}; // class DataSentEvent


} // namespace tab

#endif // __TCP_SERVER_EVENTS__