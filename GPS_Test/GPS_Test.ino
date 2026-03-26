#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include "esp_bt.h"

// ===== GPS UART CONFIG =====
#define GPS_RX 17   // ESP32 RX (connect to GPS TX)
#define GPS_TX 16   // ESP32 TX (connect to GPS RX)
#define GPS_BAUD 9600

HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

unsigned long lastPrint = 0;
unsigned long lastDataTime = 0;
unsigned long totalBytes = 0;

void setup() {
  Serial.begin(115200);

  // Disable Bluetooth
  esp_bt_controller_disable();

  Serial.println("\n===== ESP32 GPS DEBUG START =====");

  // Start GPS UART
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);

  Serial.println("UART Started");
  Serial.println("Waiting for GPS data...");
}

void loop() {

  // ===== READ GPS =====
  while (gpsSerial.available()) {
    char c = gpsSerial.read();

    // Print RAW data
    Serial.print(c);

    gps.encode(c);

    totalBytes++;
    lastDataTime = millis();
  }

  // ===== DEBUG STATUS EVERY 2 SEC =====
  if (millis() - lastPrint > 2000) {
    lastPrint = millis();

    Serial.println("\n\n===== DEBUG STATUS =====");

    Serial.print("Total Bytes Received: ");
    Serial.println(totalBytes);

    Serial.print("Last Data Received (ms ago): ");
    Serial.println(millis() - lastDataTime);

    // Check if data is coming
    if ((millis() - lastDataTime) > 3000) {
      Serial.println("⚠️ NO DATA FROM GPS");
    } else {
      Serial.println("✅ GPS DATA FLOWING");
    }

    // Location
    if (gps.location.isValid()) {
      Serial.print("LAT: ");
      Serial.println(gps.location.lat(), 6);

      Serial.print("LON: ");
      Serial.println(gps.location.lng(), 6);
    } else {
      Serial.println("Location: NOT AVAILABLE");
    }

    // Satellites
    if (gps.satellites.isValid()) {
      Serial.print("Satellites: ");
      Serial.println(gps.satellites.value());
    } else {
      Serial.println("Satellites: NOT AVAILABLE");
    }

    // Fix check
    if (gps.location.isUpdated()) {
      Serial.println("✅ NEW GPS UPDATE RECEIVED");
    } else {
      Serial.println("⚠️ NO NEW GPS UPDATE");
    }

    Serial.println("========================\n");
  }
}