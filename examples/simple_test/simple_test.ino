#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "wifi_manager.h"

#define WIFI_LED_PIN 2  // GPIO2 for onboard LED (or change to an external pin)

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

void onWiFiConnected() {
  Serial.println("[Callback] Connected to WiFi successfully.");
  Serial.print("Signal strength: ");
  Serial.print(getWiFiSignalStrength());
  Serial.println(" dBm");

  Serial.print("Local IP: ");
  Serial.println(getIPAddress());
}

void onWiFiFailed() {
  Serial.println("[Callback] WiFi connection failed.");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  setWiFiStatusLED(WIFI_LED_PIN);
  connectToWiFi(ssid, password, onWiFiConnected, onWiFiFailed);
}

void loop() {
  handleWiFi();
  // Other tasks
}
