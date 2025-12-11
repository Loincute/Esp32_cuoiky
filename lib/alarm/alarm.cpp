#include "alarm.h"

const float TILT_THRESHOLD_G = 0.7f;
const float LIFT_THRESHOLD_G = 0.6f;
const unsigned long ALARM_SILENCE_AFTER_MS = 120000;
const unsigned long BUZZ_ON_MS = 150;
const unsigned long BUZZ_OFF_MS = 150;

bool alarmState = false;
unsigned long alarmStartTime = 0;
unsigned long buzzerToggleMillis = 0;
bool buzzerOn = false;

void alarmOn(int ledPin, int buzzerPin) {
    if (!alarmState) {
        alarmState = true;
        alarmStartTime = millis();
        digitalWrite(ledPin, HIGH);
        buzzerOn = true;
        digitalWrite(buzzerPin, HIGH);
        buzzerToggleMillis = millis();
    }
}

void alarmOff(int ledPin, int buzzerPin) {
    if (alarmState) {
        alarmState = false;
        digitalWrite(ledPin, LOW);
        buzzerOn = false;
        digitalWrite(buzzerPin, LOW);
    }
}

void alarmUpdate() {
    if (!alarmState) return;
    unsigned long now = millis();
    if (buzzerOn) {
        if (now - buzzerToggleMillis >= BUZZ_ON_MS) {
            buzzerOn = false;
            buzzerToggleMillis = now;
        }
    } else {
        if (now - buzzerToggleMillis >= BUZZ_OFF_MS) {
            buzzerOn = true;
            buzzerToggleMillis = now;
        }
    }
    if (alarmState && now - alarmStartTime >= ALARM_SILENCE_AFTER_MS) {
        alarmState = false;
    }
}

bool detectTilt(float ax, float ay) {
    return (fabs(ax) > TILT_THRESHOLD_G || fabs(ay) > TILT_THRESHOLD_G);
}

bool detectLift(float az) {
    return (az < LIFT_THRESHOLD_G);
}
