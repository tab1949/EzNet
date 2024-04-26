#include <iostream>
#include <string>
#include <ctime>

#include "EzNet/HTTP/HTTP_Response.hpp"

using namespace std;
using namespace tab;

int main() {
    string rawString(
        "HTTP/1.1 200 OK\r\n"
        "Server: server/0.0\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 10\r\n"
        "\r\n"
        "123456789");
    cout << "Raw string: " << endl;
    cout << rawString << endl;
    cout << "===================" << endl;
    cout << "Parsed string: " << endl;
    cout << HttpResponse::parse(rawString).getStr() << endl;
    cout << "===================" << endl;
    const size_t tryTimes = 1000000;
    clock_t clockBegin, clockEnd;
    cout << "Benchmark start, try " << tryTimes << " times." << endl;
    clockBegin = clock();
    for (int i = 0; i < tryTimes; ++i)
        HttpResponse::parse(rawString);
    clockEnd = clock();
    double timeCost = double(clockEnd - clockBegin) / 1000000;
    cout << "Result: " << timeCost << " s, " << tryTimes / timeCost << " time(s) per second." << endl;

    return 0;
}