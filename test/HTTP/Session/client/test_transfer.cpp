/**
 * @file test.cpp
 * @author 2969117392@qq.com
 * @brief Test the functions of HttpSessionClient.
 * @date 2023-09-29
 * 
 * @note If the functions about transferring were normal, this program
 * @note will print some messages (including the text of 
 * @note request, responses, etc.) to the terminal.
 * 
 */

#include <iostream>
#include <fstream>

#include "EzNet.hpp"
#include "EzNet/HTTP/HTTP_Client.hpp"

using namespace std;

int main(void) {
    tab::HttpClient cli;
    // bing.com will tell you that cn.bing.com is the correct hostname
    tab::URL tar("http://bing.com/");
    auto&& s = cli.target(tar);
    s.setWriter(tab::FileWriter("receive.html"));
    s.setKeepAlive();
    s.setAutoJump(false);
    s.addHeader(tab::HTTP::ACCEPT, "*/*");
    cout << "Task 1: request for a html document.\n";
    cout << "Target address: " << tar.getHost().getAddr().getStr() << endl;
    cout << "Start to request.\n";
    cout << "-----REQUEST-----\n";
    auto request_buffer = s.getRequest().getBuffer();
    cout << string(request_buffer.begin(), request_buffer.size()) <<endl;
    for (int i = 0; i < 1; ++i) {
        try{
            s.request();
            cout << "------RESPONSE-----\n";
            cout << s.getResponse().getStr();
        }
        catch (const exception& e) {
            cout << e.what() << endl;
            return 0;
        } 
    }
    cout << "Task 2: request for a jpg image.\n";
    try{
        cout << cli.target(tab::URL("https://cn.bing.com/th?id=OHR.WinterberryBush_ZH-CN1414026440_UHD.jpg"))
        .setWriter(tab::FileWriter("receive.jpg"))
        .request()
        .getStr();
    } 
    catch (const exception& e) {
        cout << e.what() <<endl;
    }
    cout << "Latest error: " << tab::SystemEnv::GetError() << endl;
    return 0;
}