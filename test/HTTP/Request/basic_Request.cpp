#include <iostream>
#include <ctime>
#include "EzNet/HTTP/HTTP_Request.hpp"

using namespace std;
using namespace tab;
using namespace HTTP;

int main(void) {
    auto req = HttpRequest::parse("POST /index.html HTTP/1.1\r\nAccept-Encoding: *\r\n\r\nABCDEFG");
    cout << "Parsed a string, result: " << endl << req.getString() << endl << "--------" << endl;
    req.cookies().add(Cookie("abc=def"));
    cout << "Added a cookie, result: " << endl << req.getString() << endl << "--------" << endl;
    req.cookies().add(Cookie("x=y"));
    cout << "Added a cookie, result: " << endl << req.getString() << endl << "--------" << endl;
    return 0;
}