#ifndef __HTTP_PROTOCOL_HPP__
#define __HTTP_PROTOCOL_HPP__

#include <string>

#include "EzNet/Utility/Network/URL.hpp"

namespace tab {

namespace HTTP {

/// @brief To describe the HTTP version.
using ProtocolVersion = std::string;

/// @brief To describe a URI.
using URI = URL::Path;

using ResPhrase = std::string;

} // namespace HTTP

} // namespace tab

#endif // __HTTP_PROTOCOL_HPP__