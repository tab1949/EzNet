
#include <iostream>
#include <fstream>

#include "EzNet.hpp"
#include "EzNet/HTTP/HTTP_Client.hpp"

using namespace std;
using namespace tab;

int main(void) {
    cout << "Construct HttpClient: ";
    HttpClient httpClient;
    auto sessionA = httpClient.target(URL("https://baidu.com/"));
    cout << "OK.\n";
    sessionA.setWriter(NullWriter());
    sessionA.setAutoJump(false);
    try{
        sessionA.request();
        cout << "Request -> baidu.com: \n";
        cout << sessionA.getResponse().getStr() << endl;
        cout << "\nRequest: OK.\n";
    }
    catch (const exception& e) {
        cout << "Request failed. Exception: " << e.what() << endl;
    }
    cout << "Move: ";
    HttpSessionClient sessionB(move(sessionA));
    cout << "OK.\n";
    try{
        sessionB.request();
        cout << "Request -> baidu.com: \n";
        cout << sessionB.getResponse().getStr() << endl;
        cout << "\nRequest after moving: OK.\n";
    }
    catch (const exception& e) {
        cout << "Request failed. Exception: " << e.what() << endl;
    }
    return 0;
}