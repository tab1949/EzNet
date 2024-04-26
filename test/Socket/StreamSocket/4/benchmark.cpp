#include <iostream>
#include <string>
#include <chrono>

#include "EzNet/Basic/platform.h"
#ifdef EN_OPENSSL
#undef EN_OPENSSL
#endif

#include "EzNet.hpp"

using namespace std;
using namespace std::chrono;
using namespace tab;

int main(void) {
    cout << "IPv4 socket benchmark test." << endl;
    cout << "How many times to try (default=1,000,000): ";
    size_t try_times = 1000000;
    string str;
    for (char temp;;) {
        cin.get(temp);
        if (temp != '\n') {
            str.push_back(temp);
        }
        else {
            try {
                try_times = stoull(str);
            }
            catch (...) {

            }
            break;
        }
    }
    cout << "Try " << try_times << " times." << endl;
    auto start = duration_cast<milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    for (int i = 0; i < try_times; ++i) {
        StreamSocket sock(AF_INET);
        
    }

    auto stop = duration_cast<milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    cout << "Total: " << stop-start << " ms;" << endl;
    cout << "Operation (creation) per second: " << try_times/((stop-start)/1000.0) << endl;
    cout << "Latest error: " << SystemEnv::GetError() << endl;
    return 0;
}