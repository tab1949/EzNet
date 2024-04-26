#include <iostream>
#include <ctime>
#include "EzNet/HTTP/HTTP_Request.hpp"

using namespace std;
using namespace tab;

int main(void) {
    HTTP::RequestLine req(HTTP::REQ_GET, "/", "1.1");
    cout << "Start basic test." << endl;
    cout << "Empty? " << req.empty() << endl;
    cout << "Name? " << req.getMethodName() << endl;
    cout << "Benchmark start. (Get request line for 10000000 times)" << endl;
    clock_t stop, start = clock();
    for (int i = 0; i < 10000000; ++i)
        req.get();
    stop = clock();
    cout << (stop - start) / (CLOCKS_PER_SEC / 1000) << "ms." << endl;
    cout << "\nStart copy test." << endl;
    HTTP::RequestLine reqCopy(req);
    cout << "Equal? " << (reqCopy == req) << endl;
    cout << "\nStart move test." << endl;
    HTTP::RequestLine reqMove(req), req_(req);
    cout << "Is recent one empty? " << req.empty() << endl;
    cout << "Is new one ok? " << (reqMove == req_) << endl;
    cout << req.get() << endl;
    return 0;
}