#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include <string>

namespace tab {

/**
 * @brief Transform an array to a hex string.
 * 
 * @param src Source memory.
 * @param len Length of the source.
 * @param word_len Length of every word in src. When it's set as 1, 
 *                the result will be low-byte-first, and the result 
 *                will be high-byte-first if 'word_len' equals to 'len'.
 *                Default is 1.
 * @return  The result string.
 */
std::string ArrToHex(const void* src, size_t len, int word_len = 1);


/**
 * @brief Convert the uppercase letters in 'src' to lowercase.
 * 
 * @param str The string to process.
 */
std::string& UppercaseToLower(std::string& str);


/**
 * @brief Convert the lowercase letters in 'src' to uppercase.
 * 
 * @param str The string to process.
 */
std::string& LowercaseToUpper(std::string& str);


/**
 * @brief Convert a number in string type to unsigned long long.
 * 
 * @param str String of the number, in hex.
 * @return unsigned long long 
 */
unsigned long long HexStrToULL(const std::string& str);

} // namespace tab

#endif // __TRANSFORM_HPP__