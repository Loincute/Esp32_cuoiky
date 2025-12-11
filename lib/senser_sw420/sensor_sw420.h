#pragma once
#include <Arduino.h>

int readSW420Filtered(int pin, int samples = 10, int delayMs = 2);
String vibLevelFromValue(int v);
