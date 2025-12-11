#pragma once
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>

extern WiFiClientSecure secureClient;
extern PubSubClient client;

void setupMQTT();
void connectMQTT();
void publishSensorData(String vibLevel, bool tilt, bool lift, bool alarmState,
                       float ax, float ay, float az, float gx, float gy, float gz);
