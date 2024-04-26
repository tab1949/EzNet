#include "EzNet/HTTP/HTTP_Response.hpp"
#include "EzNet/HTTP/HTTP_StatusLine.hpp"

#include "EzNet/Utility/General/Transform.hpp"

namespace tab {

namespace HTTP {

StatusLine StatusLine::parse(const char* line, size_t len) {
    size_t i = 0;
    size_t limit = len;
    StatusLine ret;
    
    // Get the version.
    for (size_t ver_beg = 0; i < limit; ++i) {
        if (line[i] == '/') {
            ver_beg = i + 1;
        }
        else if (line[i] == ' ') {
            ret.version_ = std::move(std::string(line + ver_beg, line + i));
            break;
        }
    }

    // Skip the spaces.
    for (; i < limit; ++i)
        if (line[i] != ' ')
            break;
    
    // Get the response-code.
    for (size_t code_beg = i; i < limit; ++i) {
        if (line[i] == ' ') {
            ret.code_ 
                = (RespCode)std::stoi(std::string(line + code_beg, line + i));
            break;
        }
    }

    // Skip the spaces and get the response-phrase.
    for (; i < limit; ++i)
        if (line[i] != ' ')
            break;

    size_t offset = 0;
    for (size_t j = limit - 1; j >= i; --j)
        if (line[j] == '\r')
            offset = limit - j;
    if (i > 0 && limit - offset > i)
        ret.phrase_ = std::move(std::string(line + i, line + len - offset));

    return ret;
}

} // namespace HTTP

HttpResponse HttpResponse::parse(const std::string& raw) {
    size_t i = 0, j;
    size_t limit = raw.size();
    HttpResponse ret;

    for (; i < limit; ++i)
        if (raw[i] == '\r')
            break;

    if (i > 0)
        ret.status_line_
            = std::move(
                HTTP::StatusLine(std::string(raw.begin(), raw.begin() + i)));
    else
        return ret;

    for (i += 2, j = i; j < limit; ++j) {
        if (raw[j] == '\r') {
            std::string&& line = std::string(raw.begin() + i, raw.begin() + j);
            i = j + 2;
            if (!line.empty()) {
                auto&& header = HTTP::Header::parse(line);
                if (header.type() == HTTP::Header::Type::Common) {
                    auto& converted_header = header.to<HTTP::CommonHeader>();
                    if (converted_header.getKey() == HTTP::SET_COOKIE)
                        ret.getCookies()
                           .add(HTTP::Cookie::parse(
                                converted_header.getValue()));
                    else
                        ret.addHeader(std::move(header));
                }
                else {
                    std::string key_str(std::move(header.getKeyString()));
                    if (UppercaseToLower(key_str) == "set-cookie")
                        ret.getCookies()
                           .add(HTTP::Cookie::parse(header.getValue()));
                    else
                        ret.addHeader(std::move(header));
                }
                ++j;
            }
            else {
                break;
            }
        }
    }

    if (i > 0 && i < raw.size())
        ret.body_ = std::move(std::string(raw.begin() + i, raw.end()));

    return ret;
}

std::string HttpResponse::getStr(void) const {
    std::string ret;
    ret.append(status_line_.getStr());
    ret.append(headers_.getStr());
    if (cookies_.size() > 0)
        ret.append(cookies_.getSettingString());
    ret.append("\r\n");
    ret.append(body_);
    return ret;
}

} // namespace tab