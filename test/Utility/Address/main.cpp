#include <iostream>
#include "EzNet/Basic/platform.h"
#ifdef EN_OPENSSL
#undef EN_OPENSSL
#endif 
#include "EzNet.hpp"
#include "EzNet/Utility/Address.hpp"

using namespace std;
using namespace tab;

int main(void) {
    StreamSocket6 s;
    cout << "Local address convertion test: " << endl;
    cout << "Address::GetLocalAddress(): " << Address::GetLocalAddress().getStr() << endl;
    cout << "Address::GetAddrByName(\"127.0.0.1\"): " << Address::GetAddrByName("127.0.0.1").getStr() << endl;
    cout << "Address::GetAddrByName(\"fe80::1\"): " << Address::GetAddrByName("fe80::1").getStr() << endl;
    cout << "Domain name test: " << endl;
    cout << "Address::GetAddrByName(\"bing.com\"): " << Address::GetAddrByName("bing.com").getStr() << endl;
    cout << "Address::GetAddrByName(\"baidu.com\"): " << Address::GetAddrByName("baidu.com").getStr() << endl;
    
    cout << "Latest error: " << SystemEnv::GetError() << endl;
    return 0;
}