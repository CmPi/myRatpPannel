# 1 "c:\\users\\christ~1\\appdata\\local\\temp\\tmpdp_ctc"
#include <Arduino.h>
# 1 "C:/Users/Christophe/Documents/PlatformIO/Projects/myRatpPannel/src/debug.ino"
# 17 "C:/Users/Christophe/Documents/PlatformIO/Projects/myRatpPannel/src/debug.ino"
#if DEBUG_SUPPORT



#include <stdio.h>

#include <stdarg.h>

#include <EEPROM.h>



extern "C" {

    #include "user_interface.h"

}



#if DEBUG_UDP_SUPPORT

#include <WiFiUdp.h>

WiFiUDP _udp_debug;

#endif
void _debugSend(char * message);
void debugSend(const char * format, ...);
void debugSend_P(PGM_P format_P, ...);
void debugClearCrashInfo();
void debugDumpCrashInfo();
#line 47 "C:/Users/Christophe/Documents/PlatformIO/Projects/myRatpPannel/src/debug.ino"
void _debugSend(char * message) {



    #if DEBUG_ADD_TIMESTAMP

        static bool add_timestamp = true;

        char timestamp[10] = {0};

        if (add_timestamp) snprintf_P(timestamp, sizeof(timestamp), PSTR("[%06lu] "), millis() % 1000000);

        add_timestamp = (message[strlen(message)-1] == 10) || (message[strlen(message)-1] == 13);

    #endif



    #if DEBUG_SERIAL_SUPPORT

        #if DEBUG_ADD_TIMESTAMP

            DEBUG_PORT.printf(timestamp);

        #endif

        DEBUG_PORT.printf(message);

    #endif



    #if DEBUG_UDP_SUPPORT

        #if SYSTEM_CHECK_ENABLED

        if (systemCheck()) {

        #endif

            _udp_debug.beginPacket(DEBUG_UDP_IP, DEBUG_UDP_PORT);

            #if DEBUG_ADD_TIMESTAMP

                _udp_debug.write(timestamp);

            #endif

            _udp_debug.write(message);

            _udp_debug.endPacket();

        #if SYSTEM_CHECK_ENABLED

        }

        #endif

    #endif



    #if DEBUG_TELNET_SUPPORT

        #if DEBUG_ADD_TIMESTAMP

            _telnetWrite(timestamp, strlen(timestamp));

        #endif

        _telnetWrite(message, strlen(message));

    #endif





}



void debugSend(const char * format, ...) {



    va_list args;

    va_start(args, format);

    char test[1];

    int len = ets_vsnprintf(test, 1, format, args) + 1;

    char * buffer = new char[len];

    ets_vsnprintf(buffer, len, format, args);

    va_end(args);



    _debugSend(buffer);



    delete[] buffer;



}



void debugSend_P(PGM_P format_P, ...) {



    char format[strlen_P(format_P)+1];

    memcpy_P(format, format_P, sizeof(format));



    va_list args;

    va_start(args, format_P);

    char test[1];

    int len = ets_vsnprintf(test, 1, format, args) + 1;

    char * buffer = new char[len];

    ets_vsnprintf(buffer, len, format, args);

    va_end(args);



    _debugSend(buffer);



    delete[] buffer;



}
# 211 "C:/Users/Christophe/Documents/PlatformIO/Projects/myRatpPannel/src/debug.ino"
#define SAVE_CRASH_EEPROM_OFFSET 0x0100
# 247 "C:/Users/Christophe/Documents/PlatformIO/Projects/myRatpPannel/src/debug.ino"
#define SAVE_CRASH_CRASH_TIME 0x00

#define SAVE_CRASH_RESTART_REASON 0x04

#define SAVE_CRASH_EXCEPTION_CAUSE 0x05

#define SAVE_CRASH_EPC1 0x06

#define SAVE_CRASH_EPC2 0x0A

#define SAVE_CRASH_EPC3 0x0E

#define SAVE_CRASH_EXCVADDR 0x12

#define SAVE_CRASH_DEPC 0x16

#define SAVE_CRASH_STACK_START 0x1A

#define SAVE_CRASH_STACK_END 0x1E

#define SAVE_CRASH_STACK_TRACE 0x22
# 281 "C:/Users/Christophe/Documents/PlatformIO/Projects/myRatpPannel/src/debug.ino"
extern "C" void custom_crash_callback(struct rst_info * rst_info, uint32_t stack_start, uint32_t stack_end ) {
# 293 "C:/Users/Christophe/Documents/PlatformIO/Projects/myRatpPannel/src/debug.ino"
    uint32_t crash_time = millis();

    EEPROM.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_CRASH_TIME, crash_time);





    EEPROM.write(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_RESTART_REASON, rst_info->reason);

    EEPROM.write(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EXCEPTION_CAUSE, rst_info->exccause);





    EEPROM.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC1, rst_info->epc1);

    EEPROM.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC2, rst_info->epc2);

    EEPROM.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC3, rst_info->epc3);

    EEPROM.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EXCVADDR, rst_info->excvaddr);

    EEPROM.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_DEPC, rst_info->depc);





    EEPROM.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_START, stack_start);

    EEPROM.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_END, stack_end);





    int16_t current_address = SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_TRACE;

    for (uint32_t i = stack_start; i < stack_end; i++) {

        byte* byteValue = (byte*) i;

        EEPROM.write(current_address++, *byteValue);

    }



    EEPROM.commit();



}
# 357 "C:/Users/Christophe/Documents/PlatformIO/Projects/myRatpPannel/src/debug.ino"
void debugClearCrashInfo() {

    uint32_t crash_time = 0xFFFFFFFF;

    EEPROM.put(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_CRASH_TIME, crash_time);

    EEPROM.commit();

}
# 375 "C:/Users/Christophe/Documents/PlatformIO/Projects/myRatpPannel/src/debug.ino"
void debugDumpCrashInfo() {



    uint32_t crash_time;

    EEPROM.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_CRASH_TIME, crash_time);

    if ((crash_time == 0) || (crash_time == 0xFFFFFFFF)) {

        DEBUG_MSG_P(PSTR("[DEBUG] No crash info\n"));

        return;

    }



    DEBUG_MSG_P(PSTR("[DEBUG] Crash at %lu ms\n"), crash_time);

    DEBUG_MSG_P(PSTR("[DEBUG] Reason of restart: %u\n"), EEPROM.read(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_RESTART_REASON));

    DEBUG_MSG_P(PSTR("[DEBUG] Exception cause: %u\n"), EEPROM.read(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EXCEPTION_CAUSE));



    uint32_t epc1, epc2, epc3, excvaddr, depc;

    EEPROM.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC1, epc1);

    EEPROM.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC2, epc2);

    EEPROM.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EPC3, epc3);

    EEPROM.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_EXCVADDR, excvaddr);

    EEPROM.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_DEPC, depc);

    DEBUG_MSG_P(PSTR("[DEBUG] epc1=0x%08x epc2=0x%08x epc3=0x%08x\n"), epc1, epc2, epc3);

    DEBUG_MSG_P(PSTR("[DEBUG] excvaddr=0x%08x depc=0x%08x\n"), excvaddr, depc);



    uint32_t stack_start, stack_end;

    EEPROM.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_START, stack_start);

    EEPROM.get(SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_END, stack_end);

    DEBUG_MSG_P(PSTR("[DEBUG] >>>stack>>>\n[DEBUG] "));

    int16_t current_address = SAVE_CRASH_EEPROM_OFFSET + SAVE_CRASH_STACK_TRACE;

    int16_t stack_len = stack_end - stack_start;

    uint32_t stack_trace;

    for (int16_t i = 0; i < stack_len; i += 0x10) {

        DEBUG_MSG_P(PSTR("%08x: "), stack_start + i);

        for (byte j = 0; j < 4; j++) {

            EEPROM.get(current_address, stack_trace);

            DEBUG_MSG_P(PSTR("%08x "), stack_trace);

            current_address += 4;

        }

        DEBUG_MSG_P(PSTR("\n[DEBUG] "));

    }

    DEBUG_MSG_P(PSTR("<<<stack<<<\n"));



}

#endif