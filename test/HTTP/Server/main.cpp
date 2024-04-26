#include <iostream>

#include "EzNet.hpp"
#include "EzNet/HTTP/HTTP_Server.hpp"

using namespace std;

int main() {
    tab::HttpServer server;

    cout << "This program will listen on: " << endl;
    cout << "  " << server.configTCP().listen_address.getStr() << endl;
    server.registerEvent<tab::HttpRequestReceivedEvent>([](tab::HttpRequestReceivedEvent& e){
        auto&& uri = e.getRequest().getURI();
        auto& resp = e.getResponse();
        unsigned long long num1 = 0, num2 = 0;
        try {
            auto pos_plus = uri.find_first_of('+');
            num1 = stoull(uri.substr(1, pos_plus));
            num2 = stoull(uri.substr(pos_plus + 1, uri.size() - pos_plus - 1));
        }
        catch (const exception& e) {
            resp.getStatusLine().setCode(tab::HTTP::RespCode::BAD_REQUEST);
            resp.getStatusLine().setPhrase("Bad Request");
            resp.getBody().append("<HTML><HEAD><TITLE>Bad Request</TITLE></HEAD><BODY><H1>");
            resp.getBody().append(e.what());
            resp.getBody().append("</H1></BODY></HTML>");
            return;
        }
        resp.getBody().append("<HTML><HEAD><TITLE>HTTP Server Test</TITLE></HEAD><BODY><H1>");
        resp.getBody().append(to_string(num1 + num2));
        resp.getBody().append("</H1></BODY></HTML>");
    });
    server.start();
    cout << "Started." << endl;

    cin.get();
    cout << "Stopping..." << endl;
    server.stop();
    cout << "Latest error: " << tab::SystemEnv::GetError() << endl;
    return 0;
}