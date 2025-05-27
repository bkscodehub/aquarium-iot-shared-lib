#include <ESP8266WiFi.h>
#include <time.h>          // For NTP

// Time has to be obtained from NTP, because of CA expiration date validation.
void setClock() {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // Keep UTC
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
    Serial.print("Current time (UTC): ");
    Serial.print(asctime(&timeinfo));
  }
  
// Timestamp in IST
String getTimestamp() {
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo); // Get local time (IST)
    char buffer[25];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S+05:30", &timeinfo);  // ISO 8601 IST format
    return String(buffer);
  }
  
  int getHourNow() {
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);  // Retrieves local time (IST if configured)
  
    return timeinfo.tm_hour;
  }
  
  int getMinuteNow() {
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);  // Retrieves local time (IST if configured)
    
    return timeinfo.tm_min;
  }
    