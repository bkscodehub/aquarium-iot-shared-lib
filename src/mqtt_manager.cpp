#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "mqtt_manager.h"
#include "light_controller.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Store topic subscriptions
const char* topicCmd = nullptr;
const char* topicSchedule = nullptr;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  if (String(topic) == topicCmd) {
    handleCommandMessage(msg);
  } else if (String(topic) == topicSchedule) {
    handleScheduleMessage(msg);
  }
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientId = "aquarium-";
    clientId += String(random(0xffff), HEX);

    // These should be securely injected via GitHub Secrets or defined elsewhere
    const char* mqttUser = getenv("HIVEMQ_USERNAME");
    const char* mqttPass = getenv("HIVEMQ_PASSWORD");

    if (mqttClient.connect(clientId.c_str(), mqttUser, mqttPass)) {
      Serial.println("connected");
      mqttClient.subscribe(topicCmd);
      mqttClient.subscribe(topicSchedule);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(". Retrying in 5s...");
      delay(5000);
    }
  }
}

void setupMQTT(const char* cmdTopic, const char* scheduleTopic) {
  topicCmd = cmdTopic;
  topicSchedule = scheduleTopic;

  // Set your MQTT broker credentials and address
  const char* mqttServer = getenv("HIVEMQ_BROKER_URL");
  int mqttPort = 8883;

  mqttClient.setServer(mqttServer, mqttPort);
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
