#ifndef __TCP_SERVER_EVENTS_INTERNAL__
#define __TCP_SERVER_EVENTS_INTERNAL__

#include <functional>

#include "EzNet/Utility/Event/Event.hpp"
#include "EzNet/Utility/Event/EventMatcher.hpp"
#include "EzNet/Socket/StreamSocket.hpp"
#include "EzNet/Socket/TcpServerEvents.hpp"

namespace tab {
#ifdef _WINDOWS

#define LIMIT_DEFAULT_BUFFER 4096

class SocketContext {
public:
    SocketContext(EventMatcher& e) : matcher_(e) {
        wsabuf.buf = buffer;
        wsabuf.len = buffer_length;
    }

    ~SocketContext() {
        if (buffer_add != nullptr)
            delete[] buffer_add;
    }

    void clearBuffer() {
        std::memset(buffer, 0, buffer_length);
    }
    void clearOverlapped() {
        std::memset(&overlapped, 0, sizeof(OVERLAPPED));
    }
    void allocateAdditionalBuffer(unsigned long size) {
        buffer_add = new char[size];
        buffer_length = size;
    }

    OVERLAPPED overlapped;
    socket_t   socket;
    TcpServerEvent::OPERATION operation_required; // 1: read  0: write

    char       buffer[LIMIT_DEFAULT_BUFFER];
    ULONG      buffer_length = LIMIT_DEFAULT_BUFFER;
    ULONG      content_length = 0;
    // enable when 'buffer' is full but the data is not completely transferred
    char*      buffer_add = nullptr;
    ULONG      buffer_length_add = 0;
    ULONG      content_length_add = 0;
    // Integrate this variable into the socket context 
    // can lessen assignment operations.
    WSABUF     wsabuf; 
    // Reserved flag for higher level applications
    unsigned long long flag;

    EventMatcher& matcher_;
};

#endif // _WINDOWS

class TcpEventInternal : public Event {
public:
    constexpr static event_type_id_t GetEventTypeID() {
        return 1 << 15;
    }

protected:
    // It will be defined and specialized in 'TcpServer.cpp'
    template <class E>
    static void DumpEventData(E&, SocketContext&);

}; // class TcpEvent


class ConnectionAcceptedEventInternal : public TcpEventInternal {
public:
    constexpr static event_type_id_t GetEventTypeID() {
        return TcpEventInternal::GetEventTypeID() + 1;
    }
    
    // Implemented in 'TcpServer.cpp'
    static void Handler(ConnectionAcceptedEventInternal&); 

public:
    ConnectionAcceptedEventInternal(SocketContext& x_s, SocketContext& x_c) : 
        ctx_s(x_s), ctx_c(x_c) { }

    SocketContext& ctx_s;
    SocketContext& ctx_c;
    
}; // class ConnectionAcceptedEvent


class DataReceivedEventInternal : public TcpEventInternal {
public:
    constexpr static event_type_id_t GetEventTypeID() {
        return TcpEventInternal::GetEventTypeID() + 2;
    }

    static void Handler(DataReceivedEventInternal&); // Implemented in 'TcpServer.cpp'

public:
    DataReceivedEventInternal(SocketContext& x) : ctx_(x) { }

    SocketContext& ctx_;
}; // class DataReceivedEventInternal


class DataSentEventInternal : public TcpEventInternal {
public:
    constexpr static event_type_id_t GetEventTypeID() {
        return TcpEventInternal::GetEventTypeID() + 3;
    }
    
    // Implemented in 'TcpServer.cpp'
    static void Handler(DataSentEventInternal&); 

public:
    DataSentEventInternal(SocketContext& x) : ctx_(x) { }

    SocketContext& ctx_;
}; // class DataSentEventInternal


} // namespace tab

#endif // __TCP_SERVER_EVENTS_INTERNAL__