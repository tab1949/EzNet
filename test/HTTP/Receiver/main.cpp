#include <iostream>
#include <fstream>

#include "EzNet.hpp"
#include "EzNet/Socket/SecureSocket.hpp"
#include "EzNet/HTTP/HTTP_Request.hpp"
#include "EzNet/Utility/IO.hpp"

#include "../../../src/HTTP/Receiver.hpp"

using namespace std;

int main(void) {
    tab::URL tar("http://osu.ppy.sh/home");
    cout << "AF: " << tar.getHost().getAddr().getAF() << endl;
    tab::SslContext ctx;
    tab::Socket* s = new tab::StreamSocket(tar.getHost().getAddr().getAF());//tab::SecureSocket(ctx, tar.getHost().getAddr().getAF());
    tab::HttpRequest r(tab::HTTP::REQ_GET, tar.getPath());
    r.addHeader(tab::HTTP::HOST, tar.getHostName());
    r.addHeader(tab::HTTP::ACCEPT_ENCODING, "*");
    r.addHeader(tab::HTTP::ACCEPT_LANGUAGE, "zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6,ja;q=0.5");
    cout << "Target address family: " << tar.getHost().getAddr().getAF() << endl;
    cout << "Start to request.\n";
    cout << "-----REQUEST-----\n";
    auto request_buffer = r.getBuffer();
    cout << string(request_buffer.begin(), request_buffer.size()) <<endl;
    try {
        cout << "connect(): " << s->connect(tar.getHost()) << endl;

    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    cout << "send(): " << s->send(request_buffer.begin(), request_buffer.size()) << endl;
    char buffer[4096];
    tab::Receiver receiver(buffer, 4096, tab::FileWriter("recv.html"));
    try {
        receiver.receive(s);
    }
    catch (const exception& e) {
        cout << "Receive: " << e.what() << endl;
    }
    cout << endl;
    cout << "Response: " << receiver.resp_.getStr() << endl;
    cout << "Cookies: " << endl << receiver.resp_.getCookies().getSettingString() << endl;
    delete s;
    cout << "Latest error: " << tab::SystemEnv::GetError() << endl;
    return 0;
}
