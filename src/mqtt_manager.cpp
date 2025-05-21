#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "mqtt_manager.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Store topic subscriptions
const char* topicCmd = nullptr;
const char* topicSchedule = nullptr;
const char* mqttUserId = nullptr;
const char* mqttPass = nullptr;

const MqttCallbackEntry* callbackEntries = nullptr;
int entryCount = 0;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  String topicStr(topic);
  for (int i = 0; i < entryCount; ++i) {
    if (String(callbackEntries[i].topic) == topicStr && callbackEntries[i].callback) {
      callbackEntries[i].callback(topicStr, msg);
      return;
    }
  }

  Serial.print("No callback for topic: ");
  Serial.println(topicStr);
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");

    String clientId = "aquarium-ESPClient-";
    clientId += String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str(), mqttUserId, mqttPass)) {
      Serial.println("connected");

      // Re-subscribe to topics
      for (int i = 0; i < entryCount; ++i) {
        mqttClient.subscribe(callbackEntries[i].topic);
        Serial.print("Subscribed to: ");
        Serial.println(callbackEntries[i].topic);
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(". Retrying in 5s...");
      delay(5000);
    }
  }
}

void initMQTT(const char* broker, const char* userId, const char* password, int port, const MqttCallbackEntry* entries, int count) {
  callbackEntries = entries;
  entryCount = count;

  mqttUserId = userId;
  mqttPass = password;

  mqttClient.setServer(broker, port);
  mqttClient.setCallback(mqttCallback);
}

void loopMQTT() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
}

void publishMessage(const char* topic, const ArduinoJson::DynamicJsonDocument& doc) {
  char buffer[256];
  serializeJson(doc, buffer);
  mqttClient.publish(topic, buffer);
}
