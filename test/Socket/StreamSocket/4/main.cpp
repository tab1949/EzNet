#include <iostream>
#include <thread>
#include <mutex>

#include "EzNet/Basic/platform.h"
#ifdef EN_OPENSSL
#undef EN_OPENSSL
#endif

#include "EzNet.hpp"

using namespace std;
using namespace tab;

int main(void) {
    cout << "This is the test of StreamSocket4." << endl;
    cout << "1. Try to make some TCP sockets which uses IPv4." << endl;
    try{
        StreamSocket4 client;
        ServerSocket4 server;
        cout << "Successful." << endl;
        cout << "2. Try to bind with a port(1145)." << endl;
        if (server.bind(1145))
            cout << "Successful." << endl;
        else
            throw "bind()";
        cout << "3. Try to listen." << endl;
        if (server.listen())
            cout << "Successful." << endl;
        else
            throw "listen()";
        cout << "4. Try to connect/accept." << endl;
        mutex mtx;

        thread t1([&]{
            try {
                auto s = server.accept();
                mtx.lock();
                cout << "Server: successfully accepted." << endl;
                mtx.unlock();
                auto msg = s.recv();
                mtx.lock();
                cout << "Server: Message received: " << msg << endl;
                mtx.unlock();
            }
            catch (exception e) {
                mtx.lock();
                cout << "Server: Exception: " << e.what() << endl;
                mtx.unlock();
            }
        });

        this_thread::sleep_for (chrono::milliseconds(100));
        client.connect(Address(Address::LOCAL_ADDRESS_V4).setPort(1145));
        mtx.lock();
        cout << "Client: successfully connected." << endl;
        mtx.unlock();
        cout << "5. Try to send/recv." << endl;
        if (client.send("<This is a message>") == 19) {
            mtx.lock();
            cout << "Client: successfully sent 19 bytes." << endl;
            mtx.unlock();
        }
        else {
            throw "send()";
        }
        this_thread::sleep_for (chrono::milliseconds(100));
        if (t1.joinable())
            t1.join();
    }
    catch (const exception& e) {
        cout << "Failed, exception: " << e.what() << endl;
    }
    cout << "Latest error: " << SystemEnv::GetError() << endl;
    return 0;
}