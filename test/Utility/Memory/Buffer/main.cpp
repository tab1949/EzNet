#include <iostream>
#include <string>

#include "EzNet/Utility/Memory.hpp"

int main(int argc, const char** argv) {
    using namespace std;
    tab::Buffer buffer(8);
    cout << endl;
    cout << "Created a new Buffer, size: " << buffer.size() << ", actual size: " << buffer.getMemorySize() << ". Expected: 0; 8" << endl;
    
    buffer.resize(5);
    auto ite = buffer.begin();
    string test_str("Hello");
    for (int i = 0; i < test_str.size(); ++i) {
        ite[i] = test_str[i];
    }
    cout << endl;
    cout << "Use the iterator to access the memory, result: ";
    for (; ite != buffer.end(); ++ite) {
        cout << *ite;
    }
    cout << ". Expected: " << test_str << endl;

    buffer.clear();
    cout << endl;
    cout << "Cleared the buffer, size: " << buffer.size() << ". Expected: 0" << endl;

    buffer.append('A');
    cout << endl;
    cout << "Appended a char, size: " << buffer.size() << ", result: " << string(buffer.begin(), buffer.size()) << ". Expected: 1; A" << endl;
    
    test_str = "BCDEFGHIJKLMN";
    buffer.append(test_str.begin(), test_str.end());
    cout << endl;
    cout << "Appended a string, size: " << buffer.size() << ", result: " << string(buffer.begin(), buffer.size()) << ". Expected: 14; ABCDEF" << endl;

    buffer.resize(4);
    cout << endl;
    cout << "Resized, result: " << buffer.size() << ". Expected: 4" << endl;
    buffer.resize(8);
    cout << "Resized, result: " << buffer.size() << ". Expected: 8" << endl;
    buffer.resize(16);
    cout << "Resized, result: " << buffer.size() << ". Expected: 16" << endl;


    return 0;
}