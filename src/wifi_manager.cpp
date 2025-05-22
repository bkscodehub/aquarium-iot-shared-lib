#include "wifi_manager.h"

static const char* wifiSsid = nullptr;
static const char* wifiPassword = nullptr;
static WiFiCallback onConnectCallback = nullptr;
static WiFiCallback onFailCallback = nullptr;

static unsigned long lastReconnectAttempt = 0;
static const unsigned long reconnectInterval = 10000;  // 10 seconds

static int ledPin = -1;
static unsigned long lastBlinkTime = 0;
static bool ledState = false;

void setWiFiStatusLED(int pin) {
  ledPin = pin;
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // assume HIGH = off
}

void connectToWiFi(const char* ssid, const char* password, WiFiCallback onConnect, WiFiCallback onFail) {
  wifiSsid = ssid;
  wifiPassword = password;
  onConnectCallback = onConnect;
  onFailCallback = onFail;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi...");
  int retryCount = 0;

  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }
  randomSeed(micros());
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    if (ledPin != -1) digitalWrite(ledPin, LOW); // turn LED on (connected)

    if (onConnectCallback) {
      onConnectCallback();
    }
  } else {
    Serial.println("\nWiFi connection failed.");
    if (onFailCallback) {
      onFailCallback();
    }
  }
}

void handleWiFi() {
  unsigned long now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    // Blink LED if set
    if (ledPin != -1 && now - lastBlinkTime > 500) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState ? LOW : HIGH);  // Toggle LED
      lastBlinkTime = now;
    }

    // Debounced reconnect
    if (now - lastReconnectAttempt > reconnectInterval) {
      Serial.println("[WiFi] Disconnected. Attempting reconnection...");
      WiFi.disconnect();
      WiFi.begin(wifiSsid, wifiPassword);
      lastReconnectAttempt = now;
    }
  } else {
    // If reconnected, turn LED solid ON
    if (ledPin != -1) digitalWrite(ledPin, LOW);
  }
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

int getWiFiSignalStrength() {
  return WiFi.RSSI(); // returns dBm (e.g., -70)
}

String getIPAddress() {
  return WiFi.localIP().toString();
}
