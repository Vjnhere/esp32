#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS 1000 // 1 second interval for reporting

PulseOximeter pox;
uint32_t tsLastReport = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22); // SDA on GPIO 21, SCL on GPIO 22

    Serial.print("Initializing MAX30100...");

    // Initialize the sensor
    if (!pox.begin()) {
        Serial.println("FAILED to initialize MAX30100. Please check connections.");
        while (1); // Halt the program if initialization fails
    } else {
        Serial.println("SUCCESS");
    }

    // Set IR LED current level (typically 50mA is recommended for MAX30100)
    pox.setIRLedCurrent(MAX30100_LED_CURR_50MA); 

    // Optional: Set callback for beat detection
    pox.setOnBeatDetectedCallback([]() {
        Serial.println("Beat Detected!");
    });
}

void loop() {
    // Continuously update the sensor
    pox.update();

    // Report heart rate and SpO2 values every second
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        float heartRate = pox.getHeartRate();
        float spO2 = pox.getSpO2();

        if (heartRate > 0 && spO2 > 0) {
            Serial.print("Heart rate: ");
            Serial.print(heartRate);
            Serial.print(" bpm / SpO2: ");
            Serial.print(spO2);
            Serial.println(" %");
        } else {
            Serial.println("Waiting for valid data...");
        }

        tsLastReport = millis();
    }
}
