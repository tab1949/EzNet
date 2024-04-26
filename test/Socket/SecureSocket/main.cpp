#include <iostream>
#include <fstream>
// #define CONF_OPENSSL
#include "EzNet.hpp"
#include "EzNet/Socket/SecureSocket.hpp"

using namespace std;
using namespace tab;

int main(void) {
    SslContext ctx;
    SecureSocket ss(ctx);
    cout << ss.connect(URL("https://cn.bing.com/").getHost()) << endl;
    ss.send("GET / HTTP/1.1\r\nHost: cn.bing.com\r\n\r\n");
    char buf[4096]{};
    ofstream fout("recv.txt", ios::binary);
    for (int len = ss.recv(buf, 4096); len > 0; len = ss.recv(buf, 4096)) {
        cout << len << endl;
        fout.write(buf, len);
    }
    cout << "Latest error: " << SystemEnv::GetError() << endl;
    return 0;
}