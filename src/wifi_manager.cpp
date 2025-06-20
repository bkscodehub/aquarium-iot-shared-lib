#include "wifi_manager.h"

static const char *wifiSsid = nullptr;
static const char *wifiPassword = nullptr;
static WiFiCallback onConnectCallback = nullptr;
static WiFiCallback onFailCallback = nullptr;

static unsigned long lastReconnectAttempt = 0;
static const unsigned long reconnectInterval = 15000; // 15 seconds

static int ledPin = -1;
static unsigned long lastBlinkTime = 0;
static bool ledState = false;

void setWiFiStatusLED(int pin)
{
  ledPin = pin;
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // assume HIGH = off
}

void connectToWiFi(const char *ssid, const char *password, WiFiCallback onConnect, WiFiCallback onFail)
{
  wifiSsid = ssid;
  wifiPassword = password;
  onConnectCallback = onConnect;
  onFailCallback = onFail;

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassword);
  Serial.print("Connecting to WiFi...");

  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20)
  {
    delay(1000); // Increase delay
    Serial.print(".");
    retryCount++;
    yield(); // Allows background tasks to run
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    randomSeed(micros());
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Status: ");
    Serial.print(WiFi.status());
    Serial.print(", WiFi Signal Strength: ");
    Serial.println(WiFi.RSSI()); // Returns dBm (e.g., -70)

    if (ledPin != -1)
      digitalWrite(ledPin, LOW); // turn LED low (connected)

    if (onConnectCallback)
    {
      onConnectCallback();
    }
  }
  else
  {
    Serial.println("\nWiFi connection failed.");
    if (onFailCallback)
    {
      onFailCallback();
    }
  }
}

void handleWiFi()
{
  unsigned long now = millis();

  if (WiFi.status() != WL_CONNECTED)
  {
    // Blink LED if set
    if (ledPin != -1 && now - lastBlinkTime > 500)
    {
      ledState = !ledState;
      digitalWrite(ledPin, ledState ? LOW : HIGH); // Toggle LED
      lastBlinkTime = now;
    }

    // Debounced reconnect
    if (now - lastReconnectAttempt > reconnectInterval)
    {
      Serial.println("[WiFi] Disconnected. Attempting reconnection...");
      WiFi.disconnect();
      WiFi.begin(wifiSsid, wifiPassword);
      lastReconnectAttempt = now;
    }
  }
  else
  {
    // If reconnected, turn LED solid ON
    if (ledPin != -1)
      digitalWrite(ledPin, LOW);
  }
}

bool isWiFiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

int getWiFiSignalStrength()
{
  return WiFi.RSSI(); // returns dBm (e.g., -70)
}

String getIPAddress()
{
  return WiFi.localIP().toString();
}
