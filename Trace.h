// Define functions to trace state transitions for debug purposes

#pragma once

#include <cstdint>
#include <iostream>

// #define ENABLE_TRACE // uncomment to enable trace output to the terminal

inline void TraceStartCharging(uint8_t arrayIndex, float timeMin, uint8_t numChargersInUse) {
    #ifdef ENABLE_TRACE
        std::cout <<
            "start charging - array index: " << +arrayIndex <<
            ", time: " << timeMin << " min" <<
            ", chargers in use: " << +numChargersInUse <<
            std::endl;
    #endif
}

inline void TraceFinishCharging(uint8_t arrayIndex, float timeMin, uint8_t numChargersInUse) {
    #ifdef ENABLE_TRACE
        std::cout <<
            "completed charging - array index: " << +arrayIndex <<
            ", time: " << timeMin << " min" <<
            ", chargers in use: " << +numChargersInUse <<
            std::endl;
    #endif
}

inline void TraceAddedToChargingLine(uint8_t arrayIndex, float timeMin) {
    #ifdef ENABLE_TRACE
        std::cout <<
            "placed in line - array index: " << +arrayIndex <<
            ", time: " << timeMin << " min" <<
            std::endl;
    #endif
}