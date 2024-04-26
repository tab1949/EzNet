#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <vector>

#include "EzNet/HTTP/HTTP_Cookie.hpp"

using namespace std;
using namespace tab;

using Cookie  = HTTP::Cookie;
using Cookies = HTTP::Cookies;

int main() {
    try {
        cout << "Test class 'Cookies'." << endl;
        Cookies cookies;
        Cookie c("abc=ABC; Secure");
        cout << "Add a cookie (copy): " << c.getData() << " (" << c.getKey() << ' ' << c.getValue() << ")" << endl;
        cookies.add(c);
        cout << "Result: " << endl;
        cout << "  Size: " << cookies.size() << endl;
        cout << "  Find: " << cookies.at(c.getKey()).getString() << endl;

        static const size_t count = 1000000;
        srand(time(nullptr));
        vector<Cookie> arr(count);
        for (size_t i = 0; i < count; ++i) {
            arr.push_back(move(Cookie(to_string(rand()) + '=' + to_string(rand()))));
        }
        cout << "Benchmark: add " << count << " objects and then remove them." << endl;
        clock_t start = clock();
        for (size_t i = 0; i < count; ++i)
            cookies.add(arr[i]);
        clock_t stop = clock();
        double time_second = double(stop - start) / CLOCKS_PER_SEC;
        cout << "Total: " << time_second << "s." << endl;
        cout << "Operation per second: " << count / time_second << endl;
    } 
    catch (const exception& e) {
        cout << "Exception caught, what(): " << e.what() << endl;
    }
    return 0;
}