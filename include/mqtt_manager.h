#include <ArduinoJson.h>

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

void setupMQTT(const char* cmdTopic, const char* scheduleTopic);
void loopMQTT();
void publishMessage(const char* topic, const DynamicJsonDocument& doc);

#endif
