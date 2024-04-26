#include <iostream>

#include "EzNet.hpp"
#include "EzNet/Socket/TcpServer.hpp"

using namespace std;

int main() {
    tab::TcpServer server;

    cout << "This program will listen on: " << endl;
    cout << "  " << server.configTCP().listen_address.getStr() << endl;
    
    server.registerEvent<tab::DataReceivedEvent>([](tab::DataReceivedEvent& e) {
        cout << string(e.getBuffer(), e.getContentSize()) << endl;
        memcpy(e.getBuffer(), "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 27\r\nContent-Type: text/html\r\n\r\n<html><h1>Hello</h1></html>", 111);    
        e.setContentSize(111);
        e.setNextOperation(tab::DataReceivedEvent::OP_WRITE);
    });
    server.registerEvent<tab::DataSentEvent>([](tab::DataSentEvent& e) {
        e.setNextOperation(tab::TcpServerEvent::OP_CLOSE);
    });

    server.start();
    cout << "Started." << endl;

    cin.get();
    cout << "Stopping..." << endl;
    server.stop();
    cout << "Latest error: " << tab::SystemEnv::GetError() << endl;
    return 0;
}