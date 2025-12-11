#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include "alarm.h"
#include "sensor_sw420.h"
#include "sensor_mpu6050.h"
#include "mqtt_client.h"
#include "ca_cert_emqx.h"

#define LED_PIN     13
#define BUZZER_PIN  25
#define SW420_PIN   34
#define SDA_PIN     21
#define SCL_PIN     22

const char* WIFI_SSID = "NI-Elvis-216";
const char* WIFI_PASS = "votuyen@216";

Adafruit_MPU6050 mpu;

void setup() {
    Serial.begin(115200);
    delay(300);

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    Wire.begin(SDA_PIN, SCL_PIN);
    if (!mpu.begin()) while (1) delay(1000);

    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) delay(500);

    secureClient.setCACert(emqx_ca_cert);

    setupMQTT();
    connectMQTT();
}

void loop() {
    static unsigned long lastMQTT = 0;
    static unsigned long lastSerial = 0;
    unsigned long now = millis();

    alarmUpdate();

    int vib = readSW420Filtered(SW420_PIN, 8, 2);
    String vibLevel = vibLevelFromValue(vib);

    float ax, ay, az, gx, gy, gz;
    readMPU6050(mpu, ax, ay, az, gx, gy, gz);

    bool tilt = detectTilt(ax, ay);
    bool lift = detectLift(az);

    if (antiTheftEnabled) {
        if (vibLevel == "HIGH" || tilt || lift) alarmOn(LED_PIN, BUZZER_PIN);
        else alarmOff(LED_PIN, BUZZER_PIN);
    }

    if (now - lastSerial >= 300) {
        lastSerial = now;
        Serial.printf("VIB:%s | Tilt:%s | Lift:%s | Alarm:%s | Enable:%s\n",
            vibLevel.c_str(),
            tilt?"YES":"NO",
            lift?"YES":"NO",
            alarmState?"ON":"OFF",
            antiTheftEnabled?"ON":"OFF");
    }

    if (!client.connected()) connectMQTT();
    client.loop();

    if (now - lastMQTT >= 1000) {
        lastMQTT = now;
        publishSensorData(vibLevel, tilt, lift, alarmState, ax, ay, az, gx, gy, gz);
    }
}
