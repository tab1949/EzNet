#include "EzNet/Utility/Transform.hpp"

#include <iostream>
#include <string>

using namespace tab;

using std::cout;
using std::cin;
using std::endl;
using std::string;

int main(void) {
    string strRaw("AaBbCc-_=+,<.>/?;:'\"[{]}\\|`~");
    cout << "---Test uppercase and lowercase converting--" << endl << endl;
    cout << "Raw string: " << strRaw << endl << endl;
    string strLower(strRaw);
    cout << "Lowercase:" << endl << "    " << strLower << endl << " => ";
    UppercaseToLower(strLower);
    cout << strLower << endl << endl;
    string strUpper(strRaw);
    cout << "Uppercase:" << endl << "    " << strUpper << endl << " => ";
    LowercaseToUpper(strUpper);
    cout << strUpper << endl << endl;

    cout << "Press ENTER to exit.";
    cin.get();
    return 0;
}