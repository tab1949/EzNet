#include <cstring>
#include <iostream>

#include "EzNet/Socket/TcpServer.hpp"
#include "TcpServerEventsInternal.hpp"

namespace tab {

#ifdef _WINDOWS

void PostAcceptRequest(ServerSocket* server, SocketContext* ctx) {
    socket_t client_socket
        = socket(server->getAddr().getAF(), SOCK_STREAM, IPPROTO_TCP);
    *(socket_t*)(void*)(ctx->buffer) = client_socket;
    *(ServerSocket**)(void*)(ctx->buffer + sizeof(socket_t)) = server;
    ctx->clearOverlapped();
    auto addr_len = server->getAddr().getSize() + 16;
    DWORD bytes_received = 0;
    if (AcceptEx(
            server->get(), 
            client_socket, 
            ctx->buffer + sizeof(socket_t) + sizeof(ServerSocket*), 
            0, 
            addr_len, 
            addr_len, 
            &bytes_received, 
            &ctx->overlapped) != TRUE) {
        auto err = WSAGetLastError();
        if (err != ERROR_IO_PENDING) {
            closesocket(client_socket);
            std::cerr 
                << "tab::PostAcceptRequest(): AcceptEx() failed, error: "
                << err << "." << std::endl;
        }
    }
}

void PostIORequest(SocketContext* ctx) {
    switch(ctx->operation_required) {
        case TcpServerEvent::OP_WRITE: {// if writing operation is needed
            if (WSASend(
                    ctx->socket, 
                    &ctx->wsabuf, 
                    1, 
                    NULL, 
                    0, 
                    &ctx->overlapped, 
                    NULL) != 0 && 
                WSAGetLastError() != ERROR_IO_PENDING) {
                closesocket(ctx->socket);
                delete ctx;
            }
            break;
        }
        case TcpServerEvent::OP_READ: {// if reading operation is needed
        DWORD flags = 0;
        if (WSARecv(ctx->socket, &ctx->wsabuf, 1, NULL, &flags,
                    &ctx->overlapped, NULL) != 0) {
            auto err = WSAGetLastError();
            if (err != ERROR_IO_PENDING) {
                std::cerr << "WSARecv() ERROR: " << err << std::endl;
                closesocket(ctx->socket);
                delete ctx;
            }
        }
            break;
        }
        default: { // closing operation is needed
            closesocket(ctx->socket);
            delete ctx;
        }
    }
}

#endif // _WINDOWS


void HandlerThread(TcpServer& s) {
#ifdef _WINDOWS
    DWORD byte_transferred = 0;
    ULONG_PTR completion_key;
    LPOVERLAPPED overlapped;
    while (1) {
        if (!GetQueuedCompletionStatus(
                s.completion_port_, 
                &byte_transferred, 
                &completion_key, 
                &overlapped, 
                INFINITE)) { 
            auto err = WSAGetLastError();
            switch(err) {
            case ERROR_NETNAME_DELETED:
            case ERROR_SEM_TIMEOUT:
            case ERROR_CONNECTION_ABORTED:
                if (completion_key == 0)
                    break;
                closesocket(((SocketContext*)completion_key)->socket);
                delete (SocketContext*)completion_key;
                continue;
            default:
                break;
            }
            if (err == ERROR_INVALID_HANDLE) // Completion Port is closed
                break;
            // Normally, the working threads will exit when 
            // 'byte_transferred' and 'completion_key' are both equal to 0, 
            // and should not exit when GetQueuedCompletionStatus() returns 0;
            std::cerr 
                << "tab::HandlerThread(): GetQueuedCompletionStatus() "
                "failed, error: " << err << "." << std::endl;
            continue;
        }

        auto socket_ctx = (SocketContext*)completion_key;
        
        // new connection accepted
        if (completion_key != 0 && socket_ctx->socket == s.socket_->get()) {
            auto ctx_new = new SocketContext(s.event_matcher_);
            ctx_new->socket = *(socket_t*)(void*)(socket_ctx->buffer);
            u_long param = 1;
            if (ioctlsocket(ctx_new->socket, FIONBIO, &param) != 0) {
                delete ctx_new;
                continue;
            }
            if (CreateIoCompletionPort(
                    (HANDLE)ctx_new->socket, 
                    s.completion_port_, 
                    (ULONG_PTR)ctx_new, 
                    0) == NULL) {
                delete ctx_new;
                continue;
            }
            ConnectionAcceptedEventInternal event(*socket_ctx, *ctx_new);
            s.event_matcher_.call(event);

            ctx_new->clearOverlapped();
            
            PostIORequest(ctx_new);
            continue;
        }

        if (byte_transferred == 0) {
            if (completion_key == 0) // should exit
                break;
            closesocket(socket_ctx->socket);
            delete socket_ctx;
            continue;
        }

        // IO operation completed
        if (socket_ctx->operation_required
             == TcpServerEvent::OP_READ) { // read operation completed
            DataReceivedEventInternal event(*socket_ctx);
            //if (event.ctx_.buffer_add != nullptr)
            if (event.ctx_.wsabuf.buf == event.ctx_.buffer_add)
                event.ctx_.content_length_add = byte_transferred;
            else
                event.ctx_.content_length = byte_transferred;

            s.event_matcher_.call(event);
            
            socket_ctx->clearOverlapped();
            PostIORequest(socket_ctx);
        }
        else { // write operation completed
            DataSentEventInternal event(*socket_ctx);

            s.event_matcher_.call(event);

            event.ctx_.clearOverlapped();
            PostIORequest(socket_ctx);
        }

    }
#endif // _WINDOWS
} // HandlerThread()


TcpServer& TcpServer::start() {
    if (status_ != INIT)
        throw std::logic_error("tab::TcpServer::start(): Called repeatedly.");

    // it will be 'RUNNING' after doing all steps successfully
    status_ = ENCOUNTER_ERROR; 

    checkConfig();

    socket_.reset(new ServerSocket(config_.listen_address.getAddr().getAF()));

#ifdef _WINDOWS
    u_long param = 1;
    if (ioctlsocket(socket_->get(), FIONBIO, &param) != 0)
        throw std::runtime_error(
            "tab::TcpServer::start(): "
            "Failed to change the I/O mode of the server socket.");
    
    completion_port_ 
        = CreateIoCompletionPort(
            INVALID_HANDLE_VALUE, 
            0, 
            0, 
            config_.concurrent_threads);
    if (completion_port_ == INVALID_HANDLE_VALUE)
        throw std::runtime_error(
            "tab::TcpServer::start(): Failed to create a completion port.");
#endif 

    event_matcher_.set<ConnectionAcceptedEventInternal>(
        ConnectionAcceptedEventInternal::Handler);
    if (!event_matcher_.has<ConnectionAcceptedEvent>()) {
        event_matcher_.set<ConnectionAcceptedEvent>(
            [](ConnectionAcceptedEvent& e) {
                std::memset(e.getBuffer(), 0, e.getBufferSize());
                e.setNextOperation(TcpServerEvent::OP_READ);
            });
    }
    event_matcher_.set<DataReceivedEventInternal>(
        DataReceivedEventInternal::Handler);
    if (!event_matcher_.has<DataReceivedEvent>()) {
        event_matcher_.set<DataReceivedEvent>(
            [](DataReceivedEvent& e) { 
                e.setNextOperation(TcpServerEvent::OP_CLOSE);
            }
        );
    }
    event_matcher_.set<DataSentEventInternal>(
        DataSentEventInternal::Handler);
    if (!event_matcher_.has<DataSentEvent>()) {
        event_matcher_.set<DataSentEvent>(
            [](DataSentEvent& e) { 
                e.setNextOperation(TcpServerEvent::OP_CLOSE);
            }
        );
    }
    
    socket_->bind(config_.listen_address);
    socket_->listen();

#ifdef _WINDOWS
    acceptor_ctx_ = new SocketContext(event_matcher_);
    ((SocketContext*)acceptor_ctx_)->socket = socket_->get();
    CreateIoCompletionPort(
        (HANDLE)socket_->get(), 
        completion_port_, 
        (ULONG_PTR)acceptor_ctx_, 
        0);
    PostAcceptRequest(socket_.get(), (SocketContext*)acceptor_ctx_);
#endif // _WINDOWS
    
    for (size_t i = 0, max = config_.concurrent_threads; i < max; ++ i)
        handlers_.emplace_back(
            std::unique_ptr<std::thread>(
                new std::thread(HandlerThread, std::ref(*this))));
    status_ = RUNNING;
    return *this;
} // TcpServer::start()


TcpServer& TcpServer::stop() {
    if (status_ != RUNNING)
        return *this;

#ifdef _WINDOWS
    for (size_t i = 0; i < handlers_.size(); ++i)
        PostQueuedCompletionStatus(completion_port_, 0, 0, nullptr);
    CloseHandle(completion_port_);
#endif

    socket_.reset();

    for (size_t i = 0; i < handlers_.size(); ++i) {
        if (handlers_[i]->joinable())
            handlers_[i]->join();
        handlers_[i].reset();
    }
    delete (SocketContext*)acceptor_ctx_;
    status_ = STOPPED;
    return *this;
} // TcpServer::stop()


template <class E>
void TcpEventInternal::DumpEventData(E& des, SocketContext& ctx) {
    des.buffer_           = ctx.buffer;
    des.buffer_size_      = ctx.buffer_length;
    des.buffer_add_       = ctx.buffer_add;
    des.buffer_add_size_  = ctx.buffer_length_add;
    des.content_size_     = ctx.content_length;
    des.content_size_add_ = ctx.content_length_add;

    if (ctx.wsabuf.buf == des.buffer_add_)
        des.active_buffer_ = TcpServerEventBase::EXTENDED;
    else
        des.active_buffer_ = TcpServerEventBase::DEFAULT;

    ctx.matcher_.call(des);

    // Additional buffer allocated
    if (des.allocated_buffer_add_) { 
        ctx.buffer_add = des.buffer_add_; 
        ctx.buffer_length_add = des.buffer_add_size_;
    }
    if (des.active_buffer_ == TcpServerEventBase::DEFAULT) {
        ctx.wsabuf.buf = ctx.buffer;
        if (des.next_operation_ == TcpServerEventBase::OP_WRITE)
            ctx.wsabuf.len = des.content_size_;
        else
            ctx.wsabuf.len = des.buffer_size_;
    } else {
        ctx.wsabuf.buf = ctx.buffer_add;
        if (des.next_operation_ == DataReceivedEvent::OP_WRITE)
            ctx.wsabuf.len = des.content_size_add_;
        else
            ctx.wsabuf.len = des.buffer_add_size_;
    }
    
    ctx.operation_required = des.next_operation_;
}


void ConnectionAcceptedEventInternal::Handler(
    ConnectionAcceptedEventInternal& e) {

    PostAcceptRequest(
        *(ServerSocket**)(void*)(e.ctx_s.buffer + sizeof(socket_t)), &e.ctx_s);

    ConnectionAcceptedEvent event(e.ctx_c.flag);

    TcpEventInternal::DumpEventData(event, e.ctx_c);

}

void DataReceivedEventInternal::Handler(DataReceivedEventInternal& e) {
    DataReceivedEvent event(e.ctx_.flag);

    TcpEventInternal::DumpEventData(event, e.ctx_);

}  // DataReceivedEventInternal::Handler()

void DataSentEventInternal::Handler(DataSentEventInternal& e) {
    DataSentEvent event(e.ctx_.flag);
    
    TcpEventInternal::DumpEventData(event, e.ctx_);

}

} // namespace tab