#include <iostream>
#include "EzNet.hpp"

using namespace std;
using namespace tab;

int main(void) {
    StreamSocket6 s;
    cout << "Try to connect to ipv6.bing.com: \n";
    Address&& dest = Address::GetAddrByName("ipv6.bing.com");
    dest.setPort(80);
    cout << "Address: " << dest.getStr() << endl;
    if (s.connect(dest))
        cout << "Connected successfully.\n";
    
    cout << "Latest error: " << SystemEnv::GetError() << endl;
    return 0;
}