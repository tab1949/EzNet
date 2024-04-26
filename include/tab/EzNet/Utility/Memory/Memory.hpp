#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <utility>

namespace tab {

/**
 * @brief 
 * 
 */
class Buffer {
public:
    using Iterator = char*;

public:
    Buffer() : 
        len_    (0), 
        len_mem_(0), 
        mem_    (nullptr), 
        begin_  (nullptr), 
        end_    (nullptr) {}


    /**
     * @brief Construct a new Buffer object with the given length.
     * 
     * @param len The size of this Buffer object.
     */
    Buffer(size_t len) : 
        len_    (0), 
        len_mem_(len), 
        mem_    (new char[len]), 
        begin_  (mem_.get()), 
        end_    (mem_.get()) {
            std::memset(begin_, 0, len_mem_);
        }

    
    /**
     * @brief Copy a Buffer object.
     */
    Buffer(const Buffer& buf) : Buffer(buf.len_) {
        std::copy(buf.mem_.get(), buf.mem_.get() + buf.len_, begin_);
    }


    /**
     * @brief Move a Buffer object.
     */
    Buffer(Buffer&& buf) {
        operator=(std::move(buf));
    }


    Buffer& operator=(const Buffer& buf) {
        if (buf.len_ > len_mem_) {
            this->assign(buf.len_);
        }
        else {
            len_ = buf.len_;
        }
        std::copy(buf.mem_.get(), buf.mem_.get() + buf.len_, begin_);
        return *this;
    }


    Buffer& operator=(Buffer&& buf) {
        len_     = buf.len_;
        len_mem_ = buf.len_mem_;
        begin_   = buf.begin_;
        end_     = buf.end_;
        mem_.swap(buf.mem_);
        return *this;
    }


    /**
     * @brief Assign some memory.
     * 
     * @param len The size of the new memory.
     * @return Buffer&  This object.
     */
    Buffer& assign(size_t len) {
        if (len == len_)
            return *this;
        if (len <= len_mem_) {
            len_ = len;
            return *this;
        }
        mem_.reset(new char[len]);
        len_     = 0;
        len_mem_ = len;
        begin_   = mem_.get();
        end_     = begin_;
        return *this;
    }


    /**
     * @brief Assign some memory, and initialize it with the given value.
     * 
     * @param len The size of the new memory.
     * @param val The initial value.
     * @return Buffer&  This object.
     */
    Buffer& assign(size_t len, char val) {
        this->assign(len);
        std::memset(begin_, val, len_);
        return *this;
    }


    /**
     * @brief Reset the size of this buffer, and keep the original content.
     * 
     * @warning If the new size is less than the current size, 
     *          your data may be lost.
     * 
     * @param len The new size.
     * @return Buffer&  This object.
     */
    Buffer& resize(size_t len) {
        if (len == len_)
            return *this;
        if (len <= len_mem_) {
            len_ = len;
            end_ = begin_ + len_;
            return *this;
        }
        std::shared_ptr<char[]> holder(mem_);
        size_t src_size = len_;
        // Do this only if the new size is larger than the current size.
        this->assign(len, 0);
        std::copy(holder.get(), holder.get() + src_size, begin_);
        len_ = len;
        end_ = begin_ + len_;
        return *this;
    }


    /**
     * @brief Append a byte of data to this buffer.
     * 
     * @return Buffer&  This object.
     */
    Buffer& append(char val) {
        if (len_ < len_mem_) {
            ++ len_;
            *end_ = val;
            ++ end_;
        }
        else {
            size_t new_len = len_ + 1;
            this->resize(len_ << 1);
            len_ = new_len;
            end_ = begin_ + new_len;
        }
        return *this;
    }


    /**
     * @brief Append some data to this buffer.
     * 
     * @param first Input iterator.
     * @param last  Input iterator.
     * @return Buffer&  This object.
     */
    template <class Ite>
    Buffer& append(Ite&& first, Ite&& last) {
        size_t append_size = last - first;
        size_t old_len = len_;
        if (len_ + append_size > len_mem_) {
            this->resize(len_ + append_size);
        }
        else {
            end_ += append_size;
            len_ += append_size;
        }
        std::copy(first, last, begin_ + old_len);
        return *this;
    }


    /**
     * @brief Push a byte of data in the end.
     * 
     * @param val Data.
     * @return Buffer&  This Object.
     */
    Buffer& push_back(char val) {
        this->append(val);
        return *this;
    }


    /**
     * @brief Remove the last byte in this buffer.
     * 
     * @return Buffer&  This object.
     */
    Buffer& pop_back() {
        if (len_ == 0)
            throw std::runtime_error(
                "tab::Buffer::pop_back(): This Buffer object is empty.");
        -- len_;
        -- end_;
        return *this;
    }


    /**
     * @brief Clear this buffer.
     * 
     * @return Buffer&  This object.
     */
    Buffer& clear() {
        len_ = 0;
        end_ = begin_;
        return *this;
    }


    /**
     * @brief Release the memory this buffer used.
     * 
     * @return Buffer&  This object.
     */
    Buffer& release() {
        len_     = 0;
        len_mem_ = 0;
        begin_   = nullptr;
        end_     = begin_;
        mem_.reset();
        return *this;
    }


    /**
     * @brief Get the size of this buffer.
     * @note Maybe the return value is different 
     *       from the actual size in the memory.
     * 
     */
    size_t size() {
        return len_;
    }


    /**
     * @brief Whether this buffer is empty.
     */
    bool empty() {
        return len_;
    }


    /**
     * @brief Get the actual size of this buffer in the memory.
     * 
     */
    size_t getMemorySize() {
        return len_mem_;
    }


    /**
     * @brief Get the beginning iterator.
     */
    Iterator begin() {
        return begin_;
    }


    /**
     * @brief Get the ending iterator.
     */
    Iterator end() {
        return end_;
    }


protected:
    size_t len_;
    size_t len_mem_;
    std::shared_ptr<char[]> mem_;
    Iterator begin_;
    Iterator end_;

}; // class Buffer

} // namespace tab

#endif // __MEMORY_HPP__