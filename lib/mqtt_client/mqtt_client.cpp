#include "mqtt_client.h"
#include <ArduinoJson.h>
#include "alarm.h"

WiFiClientSecure secureClient;
PubSubClient client(secureClient);

const char* MQTT_HOST = "l2bff666.ala.eu-central-1.emqxsl.com";
const int   MQTT_PORT = 8883;
const char* MQTT_USER = "BTCK";
const char* MQTT_PASS = "1";
const char* MQTT_CLIENT_ID = "ESP32_AntiTheft";

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i=0; i<length; i++) msg += (char)payload[i];

    Serial.printf("[MQTT] %s -> %s\n", topic, msg.c_str());

    if (String(topic) == "esp32/alarm/control") {
        StaticJsonDocument<64> doc;
        if (deserializeJson(doc, msg)) return;

        bool en = doc["enable"];
        antiTheftEnabled = en;

        Serial.println(en ? "Anti-theft ENABLED" : "Anti-theft DISABLED");

        if (!en) alarmOff(13, 25);  // tắt đèn & còi ngay
    }
}

void setupMQTT() {
    client.setServer(MQTT_HOST, MQTT_PORT);
    client.setCallback(mqttCallback);  // callback xử lý lệnh
}

void connectMQTT() {
    while (!client.connected()) {
        if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
            client.subscribe("esp32/alarm/control");   // SUBSCRIBE
            Serial.println("Subscribed: esp32/alarm/control");
            break;
        }
        delay(2000);
    }
}

void publishSensorData(String vibLevel, bool tilt, bool lift, bool alarmState,
                       float ax, float ay, float az, float gx, float gy, float gz) {

    StaticJsonDocument<256> doc;
    doc["vibration"] = vibLevel;
    doc["tilt"] = tilt ? "YES" : "NO";
    doc["lift"] = lift ? "YES" : "NO";
    doc["alarm"] = alarmState ? "ON" : "OFF";
    doc["enable"] = antiTheftEnabled ? "ON" : "OFF";

    JsonObject acc = doc.createNestedObject("acc");
    acc["x"] = ax; acc["y"] = ay; acc["z"] = az;

    JsonObject gyro = doc.createNestedObject("gyro");
    gyro["x"] = gx; gyro["y"] = gy; gyro["z"] = gz;

    char buf[256];
    size_t n = serializeJson(doc, buf);

    client.publish("esp32/sensor/data", buf, n);
    client.publish("esp32/status/alarm", alarmState ? "ON" : "OFF");
}
