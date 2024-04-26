#ifndef __SOCKET_EXCEPTIONS_HPP__
#define __SOCKET_EXCEPTIONS_HPP__

#include <exception>
#include <stdexcept>
#include <string>

#include "EzNet/Basic/platform.h"

namespace tab {

class InvalidAddressException : public std::logic_error {
public:
    InvalidAddressException(const std::string& str) : std::logic_error(str) {}

};

class InvalidMethodException : public std::logic_error {
public:
    InvalidMethodException(const std::string& str) : std::logic_error(str) {}

};

#ifdef EN_OPENSSL
class SslLibraryException : public std::runtime_error {
public:
    SslLibraryException(const std::string& str) : std::runtime_error(str) {}
    
}; 
#endif // EN_OPENSSL

} // namespace tab

#endif // __SOCKET_EXCEPTIONS_HPP__