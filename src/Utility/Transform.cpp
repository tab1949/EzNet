#include <climits>

#include "EzNet/Utility/General/Transform.hpp"

namespace tab {

/// @note For internal use.
#define HIGH_FOUR_BITS(BYTE) (((BYTE) >> 4) & 0x0F)
#define LOW_FOUR_BITS(BYTE) ((BYTE) & 0x0F)

std::string ArrToHex(const void* src, size_t len, int word_len) {
    auto          ptr = static_cast<const unsigned char*>(src);
    unsigned char H, L;
    std::string   ret;

    if (word_len > 1) {
        int j_beg = word_len - 1;
        size_t i = 0;
        long long j;
        for (; i < len; i += word_len) {
            for (j = j_beg; j >= 0; --j) {
                H = HIGH_FOUR_BITS(ptr[i + j]);
                L = LOW_FOUR_BITS (ptr[i + j]);
                ret.push_back(H > 9 ? ('a' + (H - 10)) : ('0' + H));
                ret.push_back(L > 9 ? ('a' + (L - 10)) : ('0' + L));
            }
        }
    }
    else if (word_len == 1) {
        for (size_t i = 0; i < len; ++i) {
            H = HIGH_FOUR_BITS(ptr[i]);
            L = LOW_FOUR_BITS (ptr[i]);
            ret.push_back(H > 9 ? ('a' + (H - 10)) : ('0' + H));
            ret.push_back(L > 9 ? ('a' + (L - 10)) : ('0' + L));
        }
    }
    // If word_len is less than 1, return an empty string.

    return ret;
} // ArrToHex


std::string& UppercaseToLower(std::string& str) {
    for (auto i = str.begin(); i != str.end(); ++i) {
        if ('A' <= *i && *i <= 'Z') {
            *i += 32;
        }
    }
    return str;
} // UppercaseToLower


std::string& LowercaseToUpper(std::string& str) {
    for (auto i = str.begin(); i != str.end(); ++i) {
        if ('a' <= *i && *i <= 'z') {
            *i -= 32;
        }
    }
    return str;
} // LowercaseToUpper


unsigned long long HexStrToULL(const std::string& str) {
    if (str.size() > LLONG_MAX)
        return 0;
    unsigned long long ret = 0;
    decltype(str.size()) exp = 0;
    for (long long i = str.size() - 1; i >= 0; --i) {
        if ('0' <= str[i] && str[i] <= '9')
            ret += (str[i] - '0') << exp;
        else if ('a' <= str[i] && str[i] <= 'f')
            ret += (str[i] - 'a' + 10) << exp;
        else if ('A' <= str[i] && str[i] <= 'F')
            ret += (str[i] - 'A' + 10) << exp;
        else if (str[i] == ' ')
            continue;
        else
            return 0;
        exp += 4;
    }
    return ret;
} // HexStrToULL(const std::string&)

} // namespace tab