//------------------------------------------------------------------------------
// Do not change this file unless you know what you are doing
// Configuration settings are in the settings.h file
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GENERAL
//------------------------------------------------------------------------------

#define ARRAYINIT(type, name, ...) \
    type name[] = {__VA_ARGS__};

//------------------------------------------------------------------------------
// TELNET
//------------------------------------------------------------------------------

#ifndef TELNET_SUPPORT
#define TELNET_SUPPORT          1               // Enable telnet support by default
#endif

#ifndef TELNET_STA
#define TELNET_STA              0               // By default, disallow connections via STA interface
#endif

#define TELNET_PORT             23              // Port to listen to telnet clients
#define TELNET_MAX_CLIENTS      1               // Max number of concurrent telnet clients

//------------------------------------------------------------------------------
// DEBUG
//------------------------------------------------------------------------------

// Serial debug log

#ifndef DEBUG_SERIAL_SUPPORT
#define DEBUG_SERIAL_SUPPORT    1               // Enable serial debug log
#endif

#ifndef DEBUG_PORT
#define DEBUG_PORT              Serial          // Default debugging port
#endif

#ifndef SERIAL_BAUDRATE
#define SERIAL_BAUDRATE         115200          // Default baudrate
#endif

#ifndef DEBUG_ADD_TIMESTAMP
#define DEBUG_ADD_TIMESTAMP     1               // Add timestamp to debug messages
                                                // (in millis overflowing every 1000 seconds)
#endif

// Second serial port (used for RX)

//#define SERIAL_RX_PORT        Serial			// This setting is usually defined
												// in the hardware.h file for those
												// boards that require it

#ifndef SERIAL_RX_BAUDRATE
#define SERIAL_RX_BAUDRATE      115200          // Default baudrate
#endif

//------------------------------------------------------------------------------

#ifndef DEBUG_TELNET_SUPPORT
#define DEBUG_TELNET_SUPPORT    TELNET_SUPPORT  // Enable telnet debug log if telnet is enabled too
#endif

#if DEBUG_TELNET_SUPPORT
#undef TELNET_SUPPORT
#define TELNET_SUPPORT          1
#endif

//------------------------------------------------------------------------------

// General debug options and macros

#define DEBUG_SUPPORT           DEBUG_SERIAL_SUPPORT || DEBUG_TELNET_SUPPORT

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

// -----------------------------------------------------------------------------
// OLED
// -----------------------------------------------------------------------------

#if OLED_ROWS_NUM==3
  #define OLED_FONT u8g2_font_6x10_mf
  #define OLED_ROWS_HEIGHT 10
  #define OLED_COLS_NUM 23
  #define OLED_COLS_WIDTH 6
#endif

#if OLED_ROWS_NUM==4
  #define OLED_FONT u8g2_font_5x8_mf
  #define OLED_ROWS_HEIGHT 8
  #define OLED_COLS_NUM 25
  #define OLED_COLS_WIDTH 5
#endif