#ifndef __EXCEPTIONS_HPP__
#define __EXCEPTIONS_HPP__

#include <exception>
#include <string>

class InvalidConversionException : public std::exception {
public:
    InvalidConversionException() = default;
    const char* what() const noexcept override {
        return "Cannot convert the given object into the specified type";
    }
};


class InvalidValueException : public std::exception {
public:
    template <typename T>
    InvalidValueException(T info) : str_(std::forward<T>(info)) {}
    const char* what() const noexcept override {
        return str_.c_str();
    }

private:
    std::string str_;
};

#endif // __EXCEPTIONS_HPP__