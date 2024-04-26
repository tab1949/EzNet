#include <iostream>
#include "EzNet.hpp"
#include "EzNet/Utility/URL.hpp"

using namespace std;
using namespace tab;

int main(void) {
    cout << "IPv6 URL resolution test: \n";
    URL url_bing("http://bing.com");
    cout << url_bing.getStr() << " :" << endl;
    cout << "Port: " << url_bing.getPort() << endl;
    cout << "Address: " << url_bing.getHost().getStr() << endl; 
    cout << "Latest error: " << SystemEnv::GetError() << endl;
    return 0;
}