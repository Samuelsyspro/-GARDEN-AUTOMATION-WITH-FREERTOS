#include <WiFi.h>
#include <wifiSetup.h>
#include "time.h"
#include "esp_sntp.h"


const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
char buffer[30];

#define TZ_America_Sao_Paulo PSTR("<-03>3")
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
}

// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval* t) {
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void Config() {
  // set notification call-back function
  sntp_set_time_sync_notification_cb(timeavailable);
  esp_sntp_servermode_dhcp(1);  // (optional)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  configTzTime(TZ_America_Sao_Paulo, ntpServer1, ntpServer2);
}
