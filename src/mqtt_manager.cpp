#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "mqtt_manager.h"
#include <time.h>          // For NTP

BearSSL::WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// HiveMQ Cloud Let's Encrypt CA certificate (hardcoded)
static char ca_cert* = nullptr;

// Store MQTT broker config
char* mqtt_server = nullptr;
int mqtt_port;
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

// Time has to be obtained from NTP, because of CA expiration date validation.
void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void initMQTT(const char* broker, const char* userId, const char* password, int port, const char ssl_cert*, const MqttCallbackEntry* entries, int count) {
  callbackEntries = entries;
  entryCount = count;

  mqttUserId = userId;
  mqttPass = password;
  ca_cert = ssl_cert;
  mqtt_server = broker;
  mqtt_port = port;

  Serial.print("Connecting to MQTT Broker: ");
  Serial.println(broker);

  BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(ca_cert);
  espClient.setTrustAnchors(serverTrustedCA);
  setClock(); // Required for X.509 validation
  mqttClient.setServer(broker, port);
  mqttClient.setCallback(mqttCallback);

  Serial.println("Connected to MQTT Broker! Callbacks registered.");
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
