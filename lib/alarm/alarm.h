#pragma once
#include <Arduino.h>

extern bool alarmState;
extern bool antiTheftEnabled;   // NEW

void alarmOn(int ledPin, int buzzerPin);
void alarmOff(int ledPin, int buzzerPin);
void alarmUpdate();
bool detectTilt(float ax, float ay);
bool detectLift(float az);
