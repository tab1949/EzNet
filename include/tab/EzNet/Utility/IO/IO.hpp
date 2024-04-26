#ifndef __IO_HPP__
#define __IO_HPP__

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

namespace tab {

class Reader; // Declaration of class Reader

class Writer; // Declaration of class Writer


/**
 * @brief Interface of all kinds of writers.
 */
class Writer {
public:
    /// @brief The only interface.
    /// @param 1 Source data to write.
    /// @param 2 Length of the data in param 1.
    /// @return Bytes written.
    virtual std::size_t operator()(const void*, std::size_t) = 0;

}; // class Writer


class NullWriter : public Writer {
public:
    std::size_t operator()(const void*, std::size_t len) override {
        return len;
    }

}; // class NullWriter


/**
 * @brief A kind of writer which writes data to the standard output stream.
 */
class StdWriter : public Writer {
public:
    std::size_t operator()(const void* data, std::size_t cnt) override {
        std::cout.write((const char*)data, cnt);
        return cnt;
    }

}; // class StdWriter


/**
 * @brief A kind of writer which writes data to an object of std::ostream.
 */
class StreamWriter : public Writer {
public:
    StreamWriter(const StreamWriter&) = default;
    StreamWriter(StreamWriter&&) = default;

    StreamWriter(std::ostream& os) : stream_(&os){ }

    virtual std::size_t operator()(const void* data, std::size_t cnt) override {
        stream_->write((const char*)data, cnt);
        return cnt;
    }

protected:
    StreamWriter() = default;
    // Only for accessing, DO NOT release it.
    std::ostream* stream_;

}; // class StreamWriter


/**
 * @brief The special case of StreamWriter, which handles file output.
 */
class FileWriter : public StreamWriter {
public:
    FileWriter(const std::string& fn) : 
        out_file_(new std::ofstream(fn, std::ios_base::binary)) {
        stream_ = out_file_.get();
    }

    void close() {
        out_file_->close();
    }

    std::size_t operator()(const void* data, std::size_t cnt) override {
        return StreamWriter::operator()((const char*)data, cnt);
    }

private:
    // Holds the ofstream created.
    // The member 'stream_' will store the value of its raw pointer,
    // and this shared_ptr will be in charge of deleting.
    std::shared_ptr<std::ofstream> out_file_;

}; // class FileWriter

class MemoryWriter : public Writer {
public:
    MemoryWriter(void* des, size_t len) : 
        dest_((char*)des), end_((char*)des + len), len_(len) { }

    std::size_t operator()(const void* data, std::size_t cnt) override {
        if (dest_ + cnt < end_) {
            std::memcpy(dest_, data, cnt);
            dest_ += cnt;
            return cnt;
        }
        else {
            if (dest_ >= end_)
                return 0;
            std::memcpy(dest_, data, end_ - dest_);
            size_t ret = end_ - dest_;
            dest_ = end_;
            return ret;
        }
    }

private:
    char* dest_;
    char* end_;
    size_t len_;

}; // class MemoryWriter

} // namespace tab

#endif // __IO_HPP__