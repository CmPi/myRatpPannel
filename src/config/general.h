



#define DEBUG_SUPPORT           DEBUG_SERIAL_SUPPORT || DEBUG_UDP_SUPPORT || DEBUG_TELNET_SUPPORT

#if DEBUG_SUPPORT
    #define DEBUG_MSG(...) debugSend(__VA_ARGS__)
    #define DEBUG_MSG_P(...) debugSend_P(__VA_ARGS__)
#endif

#ifndef DEBUG_MSG
    #define DEBUG_MSG(...)
    #define DEBUG_MSG_P(...)
#endif

// -----------------------------------------------------------------------------
// OTA
// -----------------------------------------------------------------------------

#ifndef OTA_SUPPORT
#define OTA_SUPPORT          1               // Enable telnet support by default
#endif

#define OTA_PORT                8266        // OTA port

//------------------------------------------------------------------------------
// TELNET
//------------------------------------------------------------------------------

#ifndef TELNET_SUPPORT
#define TELNET_SUPPORT          1               // Enable telnet support by default
#endif

#define TELNET_PORT             23              // Port to listen to telnet clients
#define TELNET_MAX_CLIENTS      1               // Max number of concurrent telnet clients