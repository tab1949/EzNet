#include "EzNet/Utility/Transform.hpp"

#include <iostream>
#include <string>

using namespace tab;

using std::cout;
using std::cin;
using std::endl;
using std::string;

int main(void) {
    string strRaw("123456");
    cout << "--Convert a string (hex) to a number--" << endl << endl;
    cout << "Raw string: " << strRaw << endl << endl;
    cout << "Result:" << endl << "  " << HexStrToULL(strRaw) << endl;
    cout << "Expected:" << endl << "  " << 0x123456 << endl;
    
    cout << "Other conditions: " << endl;
    cout << "  " << (HexStrToULL("FF") == 0xFF ? "successful" : "failed") << endl;
    cout << "  " << (HexStrToULL("fF") == 0xFF ? "successful" : "failed") << endl;
    cout << "  " << (HexStrToULL("00000000000fF") == 0xFF ? "successful" : "failed") << endl;
    cout << "Press ENTER to exit.";
    cin.get();
    return 0;
}