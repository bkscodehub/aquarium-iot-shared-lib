#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "mqtt_manager.h"
#include <time_utils.h>          // For NTP

BearSSL::WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// HiveMQ Cloud Let's Encrypt CA certificate (hardcoded)
static const char* ca_cert = nullptr;

// Store MQTT broker config
const char* mqtt_server = nullptr;

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
  Serial.print("Message received on topic: ");
  Serial.print(topicStr);
  Serial.print(", Message: ");
  Serial.println(msg);
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
  char err_buf[256];
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");

    String clientId = "aquariumESPClient-" + String(ESP.getChipId());
    // clientId += "-" + String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str(), mqttUserId, mqttPass)) {
      Serial.print(clientId);
      Serial.print(" connected");
      Serial.print(", State=");
      Serial.println(mqttClient.state());

      // Re-subscribe to topics
      for (int i = 0; i < entryCount; ++i) {
        mqttClient.subscribe(callbackEntries[i].topic);
        Serial.print("Subscribed to: ");
        Serial.println(callbackEntries[i].topic);
      }
      mqttClient.setKeepAlive(60);  // Set keep-alive to 60 seconds
    } else {
      Serial.print("ESP Client Id (");
      Serial.print(clientId);
      Serial.print(") failed to connect, rc=");
      Serial.print(mqttClient.state());
      espClient.getLastSSLError(err_buf, sizeof(err_buf));
      Serial.print("SSL error: ");
      Serial.println(err_buf);
      Serial.println("Retrying in 5s...");
      delay(5000);
    }
  }
}

void initMQTT(const char* broker, const char* userId, const char* password, int port, const char* ssl_cert, const MqttCallbackEntry* entries, int count) {
  callbackEntries = entries;
  entryCount = count;
  mqttUserId = userId;
  mqttPass = password;
  ca_cert = ssl_cert;
  mqtt_server = broker;
  mqtt_port = port;

  Serial.print("Initializing connection to MQTT Broker: ");
  Serial.println(broker);

  BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(ca_cert);
  espClient.setTrustAnchors(serverTrustedCA);
  setClock(); // Required for X.509 validation
  mqttClient.setServer(broker, port);
  mqttClient.setCallback(mqttCallback);

  Serial.println("Callbacks registered.");
}

void loopMQTT() {
  if (!mqttClient.connected()) {
    char err_buf[256];
    espClient.getLastSSLError(err_buf, sizeof(err_buf));
    Serial.print("SSL error: ");
    Serial.println(err_buf);

    reconnect();
  }
  mqttClient.loop();
}

void publishMessage(const char* topic, const ArduinoJson::DynamicJsonDocument& doc) {
  Serial.print("Publishing message to topic: ");
  Serial.print(topic);
  Serial.print(", Message: ");
  serializeJson(doc, Serial);  // Serialize and print JSON
  Serial.println();  // Ensure a new line for better readability

  char buffer[256];
  serializeJson(doc, buffer);
  mqttClient.publish(topic, buffer);
  Serial.println("Published message successfully");
}
