#include <Arduino.h>

extern "C" {
    #include "user_interface.h"
}

// -----------------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------------
void debugSend(const char * format, ...);
void debugSend_P(PGM_P format, ...);
