#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "EzNet/HTTP/HTTP_Cookie.hpp"

using namespace std;
using namespace tab;
using Cookie = HTTP::Cookie;

int main() {
    try {
        string raw("key123=value321; Expires=Wed, 21 Oct 2015 07:28:00 GMT; Secure; HttpOnly; Path=/; Domain=test.test");
        cout << "Try to parse a string into 'Cookie', raw:\r\n  " << raw << endl;
        Cookie&& c = Cookie::parse(raw);
        cout << "Successfully parse a string, result:" << endl;
        cout << "  String: " << c.getString() << endl;
        cout << "  Data: " << c.getData() << endl;
        cout << "  Key: " << c.getKey() << endl;
        cout << "  Value: " << c.getValue() << endl;
        cout << "  Secure: " << (c.isSecureSet() ? "true" : "false") << endl;
        cout << "  Expire time: " << c.getExpire() << endl;
        cout << "  Domain: " << c.getDomain() << endl;
        cout << "  Path: " << c.getPath() << endl;

        vector<string> raw_ex {
            "+9Et|%HpIsSe^2DN#? =;=;&c9N!SLd0[n?Secure^cSv s|`PJm`FXExpireu[/G[1Ug{h.0^u~CVQNO",
            "zxNQdPT&{r!.|/8D_/NaGD$q*9b693(wIiSczR<,o|> [{Qpv][p#{-[(b!`/3EC",
            "",
            "  ;=;",
            "==;Domain=Secure;=",
            "   ",
            ";;;=;;;;"
        };
        cout << "Extreme conditions." << endl;
        for (int i = 0; i < raw_ex.size(); ++i) {
            cout << "  " << i << ": " << raw_ex[i] << endl;
            try {
                auto&& c_ex = Cookie::parse(raw_ex[i]);
                cout << "  No exception." << endl;
                cout << "  Data: " << c_ex.getData() << endl;
            }
            catch (Cookie::InvalidCookieExpressionException) {
                cout << "  Successfully caught an exception while parsing." << endl;
            }
        }

        static const size_t try_time = 1000000;
        cout << "Benchmark: " << try_time << " times." << endl;
        clock_t start = clock();
        for (size_t i = 0; i < try_time; ++i)
            Cookie&& temp = Cookie::parse(raw);
        clock_t stop = clock();
        cout << "  Time cost: " << stop - start << " clocks." << endl;
        cout << "  Operation per second: " << try_time / (double(stop - start) / CLOCKS_PER_SEC) << endl;
    } 
    catch (const exception& e) {
        cout << "Exception caught, what(): " << e.what() << endl;
    }
    return 0;
}