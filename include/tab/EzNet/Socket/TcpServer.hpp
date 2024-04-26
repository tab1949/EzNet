/**
 * @file TcpServer.hpp
 * @author 2969117392@qq.com
 * @brief 
 * @date 2023-09-30
 * 
 */

#ifndef __TCP_SERVER__
#define __TCP_SERVER__

#include <climits>
#include <ctime>
#include <map>
#include <memory>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include "StreamSocket.hpp"
#include "TcpServerEvents.hpp"

#include "EzNet/Basic/net_type.h"
#include "EzNet/Utility/Network/Address.hpp"
#include "EzNet/Utility/Network/URL.hpp"
#include "EzNet/Utility/Event/EventMatcher.hpp"

namespace tab {

class TcpServer {
public:
    struct TcpConfig {
        TcpConfig() : listen_address(Address::GetLocalAddress()) {
            listen_address.setPort(80);
        }
        Host     listen_address;
        bool     tls_enable   = false;
        short    concurrent_threads = 1; // This variable should > 0
        int      connection_timeout_seconds = INT_MAX;
        // TODO
    }; // struct TcpConfig

public:
    TcpServer() {}
    TcpServer(const TcpConfig& cfg) : config_(cfg) {}
    
    ~TcpServer() {
        stop();
    }

    TcpConfig& configTCP() {
        return config_;
    }

    TcpServer& checkConfig() {
        if (config_.concurrent_threads <= 0)
            throw std::logic_error(
                "Tab::TcpServer::checkConfig(): "
                "Number of concurrent threads is invalid.");
        return *this;
    }

    TcpServer& start();
    TcpServer& stop();
    
    template <class Event, typename Func>
    TcpServer& registerEvent(Func f) {
        static_assert(std::is_base_of<TcpServerEvent, Event>::value,
                      "The registered event must be derived "
                      "from 'tab::TcpServerEvent'.");
        event_matcher_.set<Event>(f);
        return *this;
    }

protected:
    TcpConfig config_;
    std::unique_ptr<ServerSocket> socket_;
    std::map<std::clock_t, StreamSocket> socket_list_;
    EventMatcher event_matcher_;
    enum {INIT, RUNNING, STOPPED, ENCOUNTER_ERROR} status_ = INIT;

    // Threads
    std::vector<std::unique_ptr<std::thread>> handlers_;
    friend void HandlerThread(TcpServer&);

#ifdef _WINDOWS
    HANDLE completion_port_;
    void*  acceptor_ctx_;
#endif
#ifdef _LINUX

#endif 

}; // class TcpServer

} // namespace tab

#endif // __TCP_SERVER__