#include <ESP8266WiFi.h>
#include <time.h>          // For NTP

// Time has to be obtained from NTP, because of CA expiration date validation.
void setClock() {
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    // configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // Keep UTC
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

// Function to check if the current time falls within the range
bool isTimeInRange(int onHour, int onMinute, int offHour, int offMinute) {
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);  // Get local time (IST)

    int currentMinutes = timeinfo.tm_hour * 60 + timeinfo.tm_min;
    int onMinutes = onHour * 60 + onMinute;
    int offMinutes = offHour * 60 + offMinute;

    // Case 1: Time range does NOT span midnight (Example: 10:30 - 18:30)
    if (onMinutes <= offMinutes) {
        return currentMinutes >= onMinutes && currentMinutes < offMinutes;
    }
    // Case 2: Time range spans midnight (Example: 22:00 - 05:30)
    else {
        return currentMinutes >= onMinutes || currentMinutes < offMinutes;
    }
}
