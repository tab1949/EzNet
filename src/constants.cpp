#include "EzNet/Basic/platform.h"
#ifdef EN_OPENSSL
#undef EN_OPENSSL
#endif
#include "EzNet.hpp"

namespace tab {
    SystemEnv SystemEnv::env;
    
    const Address Address::LOCAL_ADDRESS_V4 
        = Address::GetAddrByName("127.0.0.1");
    
    const Address Address::LOCAL_ADDRESS_V6 
        = Address::GetAddrByName("::1");
} // namespace tab