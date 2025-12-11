#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include "alarm.h"
#include "sensor_sw420.h"
#include "sensor_mpu6050.h"
#include "mqtt_client.h"
#include "ca_cert_emqx.h"

/* ---------- PINS ---------- */
#define LED_PIN     13
#define BUZZER_PIN  25
#define SW420_PIN   34
#define SDA_PIN     21
#define SCL_PIN     22

/* ---------- WiFi ---------- */
const char* WIFI_SSID = "NI-Elvis-216";
const char* WIFI_PASS = "votuyen@216";

Adafruit_MPU6050 mpu;
bool mqttReady = false;

void setup() {
    Serial.begin(115200);
    delay(200);

    Serial.println("\n===== SYSTEM START =====");
    
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    /* ---------- Init MPU ---------- */
    Wire.begin(SDA_PIN, SCL_PIN);
    Serial.print("MPU6050: initializing...");
    if (!mpu.begin()) {
        Serial.println(" FAILED!");
        while(1) delay(1000);
    }
    Serial.println(" OK");

    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

    /* ---------- WiFi Connect ---------- */
    Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    /* ---------- MQTT TLS ---------- */
    secureClient.setCACert(emqx_ca_cert);
    setupMQTT();

    /* ---------- MQTT Connect ---------- */
    Serial.println("Connecting to MQTT...");
    connectMQTT();

    if (client.connected()) {
        mqttReady = true;
        Serial.println("MQTT connected!");
        Serial.println("===== STARTING DATA TRANSMISSION =====");
    }
}

void loop() {
    if (!mqttReady) return;  // CHỈ GỬI DỮ LIỆU KHI WIFI + MQTT ĐÃ OK

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

    if (vibLevel=="HIGH" || tilt || lift) alarmOn(LED_PIN, BUZZER_PIN);
    else alarmOff(LED_PIN, BUZZER_PIN);

    if (now - lastSerial >= 300) {
        lastSerial = now;
        Serial.printf(
            "SW420:%4d -> %s | Acc: %.3f %.3f %.3f | Gyro: %.2f %.2f %.2f | "
            "Tilt:%s Lift:%s | Alarm:%s\n",
            vib, vibLevel.c_str(), ax, ay, az, gx, gy, gz,
            tilt?"YES":"NO", lift?"YES":"NO", alarmState?"ON":"OFF"
        );
    }

    if (!client.connected()) {
        Serial.println("MQTT lost → reconnecting...");
        connectMQTT();
    }

    client.loop();

    if (now - lastMQTT >= 1000) {
        lastMQTT = now;
        publishSensorData(vibLevel, tilt, lift, alarmState, ax, ay, az, gx, gy, gz);
    }

    delay(10);
}
