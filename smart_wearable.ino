#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <MAX30100_PulseOximeter.h>
#include <Adafruit_BMP280.h>

// WiFi credentials
const char* ssid = "vjnhere";
const char* password = "Vijayan004";

// ThingSpeak API details
const char* server = "http://api.thingspeak.com";
String apiKey = "7FZZUB4QBHQNCACO";

// Sensor objects
PulseOximeter pox;            // MAX30100 Pulse Oximeter
Adafruit_BMP280 bmp;          // BMP280 Barometric Pressure Sensor

// Define I2C addresses
#define MAX30100_ADDR 0x57
#define BMP280_ADDR 0x76

// ECG Sensor (AD8232) analog pin 
const int ecgPin = 34;

// Define analog pins for MQ6, MQ7 sensors
#define MQ6_ANALOG_PIN 35
#define MQ7_ANALOG_PIN 36

// Define digital pins for MQ6, MQ7 sensors
#define MQ6_DIGITAL_PIN 2
#define MQ7_DIGITAL_PIN 4

// Time interval for data reading/sending (60 seconds)
const unsigned long interval = 60000;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Connecting to WiFi
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  // Initialize MAX30100 sensor
  if (!pox.begin()) {
    Serial.println("Failed to initialize MAX30100!");
    while (1);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_50MA);  // Set IR LED power

  // Initialize BMP280 sensor
  if (!bmp.begin(BMP280_ADDR)) {
    Serial.println("Failed to initialize BMP280!");
    while (1);
  }

  // Set digital pins as input
  pinMode(MQ6_DIGITAL_PIN, INPUT);
  pinMode(MQ7_DIGITAL_PIN, INPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  // Execute every 60 seconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read pulse oximeter data
    pox.update();
    float heartRate = pox.getHeartRate();
    float spo2 = pox.getSpO2();

    // Read BMP280 data
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure() / 100.0F;  // Convert to hPa

    // Read ECG sensor data (AD8232)
    int ecgValue = analogRead(ecgPin);

    // Read MQ6 and MQ7 analog and digital values
    int mq6_analog = analogRead(MQ6_ANALOG_PIN);
    int mq7_analog = analogRead(MQ7_ANALOG_PIN);
    int mq6_digital = digitalRead(MQ6_DIGITAL_PIN);
    int mq7_digital = digitalRead(MQ7_DIGITAL_PIN);

    // Print readings to Serial Monitor
    Serial.println("== Sensor Readings ==");
    Serial.print("Heart Rate: "); Serial.print(heartRate);
    Serial.print(", SpO2: "); Serial.print(spo2);
    Serial.print(", Temperature: "); Serial.print(temperature);
    Serial.print(", Pressure: "); Serial.print(pressure);
    Serial.print(", ECG Value: "); Serial.print(ecgValue);
    Serial.print(", MQ6 Analog: "); Serial.print(mq6_analog);
    Serial.print(", MQ7 Analog: "); Serial.print(mq7_analog);
    Serial.println();

    // Send data to ThingSpeak
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = String(server) + "/update?api_key=" + apiKey +
                   "&field1=" + String(heartRate) +
                   "&field2=" + String(spo2) +
                   "&field3=" + String(temperature) +
                   "&field4=" + String(pressure) +
                   "&field5=" + String(ecgValue) +
                   "&field6=" + String(mq6_analog) +
                   "&field7=" + String(mq7_analog);

      http.begin(url);
      int httpResponseCode = http.GET();
      if (httpResponseCode > 0) {
        Serial.print("Data sent to ThingSpeak, HTTP Response code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Error in sending data, code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
  }
}
