#include "mqtt_client.h"
#include <ArduinoJson.h>

WiFiClientSecure secureClient;
PubSubClient client(secureClient);

const char* MQTT_HOST = "l2bff666.ala.eu-central-1.emqxsl.com";
const int MQTT_PORT = 8883;
const char* MQTT_USER = "BTCK";
const char* MQTT_PASS = "1";
const char* MQTT_CLIENT_ID = "ESP32_AntiTheft";

void setupMQTT() {
    client.setServer(MQTT_HOST, MQTT_PORT);
}

void connectMQTT() {
    while (!client.connected()) {
        if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) break;
        delay(2000);
    }
}

void publishSensorData(String vibLevel, bool tilt, bool lift, bool alarmState,
                       float ax, float ay, float az, float gx, float gy, float gz) {

    StaticJsonDocument<256> doc;
    doc["vibration"] = vibLevel;
    doc["tilt"] = tilt?"YES":"NO";
    doc["lift"] = lift?"YES":"NO";
    doc["alarm"] = alarmState?"ON":"OFF";

    JsonObject acc = doc.createNestedObject("acc");
    acc["x"] = ax; acc["y"] = ay; acc["z"] = az;

    JsonObject gyro = doc.createNestedObject("gyro");
    gyro["x"] = gx; gyro["y"] = gy; gyro["z"] = gz;

    char buffer[256];
    size_t n = serializeJson(doc, buffer);
    client.publish("esp32/sensor/data", buffer, n);
    client.publish("esp32/status/alarm", alarmState?"ON":"OFF");
}
