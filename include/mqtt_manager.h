#pragma once

#include <ArduinoJson.h>

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

// Callback function type for incoming MQTT messages
typedef void (*MqttMessageCallback)(const String& topic, const String& payload);

// Struct to map a topic to a callback
struct MqttCallbackEntry {
  const char* topic;
  MqttMessageCallback callback;
};

// Called by application to register topics + callbacks
void initMQTT(const char *broker, const char *userId, const char *password, int port, const char ssl_cert[], const MqttCallbackEntry *entries, int count);

//void setupMQTT(const char* cmdTopic, const char* scheduleTopic);
void loopMQTT();
void publishMessage(const char* topic, const ArduinoJson::DynamicJsonDocument& doc);
String getTimestamp();  // Get datetime in UTC

#endif
