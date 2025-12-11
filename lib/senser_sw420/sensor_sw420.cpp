#include "sensor_sw420.h"

const int VIB_LOW_MAX = 200;
const int VIB_MED_MAX = 600;

int readSW420Filtered(int pin, int samples, int delayMs) {
    long total = 0;
    for (int i=0; i<samples; i++) {
        total += analogRead(pin);
        if (delayMs>0) delay(delayMs);
    }
    return total / samples;
}

String vibLevelFromValue(int v) {
    if (v < VIB_LOW_MAX) return "LOW";
    if (v < VIB_MED_MAX) return "MEDIUM";
    return "HIGH";
}
