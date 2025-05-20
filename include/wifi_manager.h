#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include <functional>

typedef std::function<void()> WiFiCallback;

void connectToWiFi(const char* ssid, const char* password, WiFiCallback onConnect = nullptr, WiFiCallback onFail = nullptr);
void handleWiFi();  // Call this in loop()
bool isWiFiConnected();
int getWiFiSignalStrength();
String getIPAddress();
void setWiFiStatusLED(int pin);

#endif
