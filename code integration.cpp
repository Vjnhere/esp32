#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ThingSpeak.h>
#include "Adafruit_TCS34725.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>

// WiFi credentials
const char* ssid = "vjnhere";
const char* password = "Vijayan004";

// ThingSpeak settings
unsigned long channelID = 2651888;
const char* apiKey = "8ZYZW3MKKJ8CJTVM";
const char* server = "api.thingspeak.com";

WiFiClient client;

// ** TCS34725 RGB Sensor **
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);
float gammaCorrection = 2.5;
float whiteBalance[3] = {255.0, 255.0, 255.0};  // Default white balance

// ** TDS Sensor **
#define TDS_PIN 32
const float VREF = 3.3;
const float ADC_RES = 4095.0;

// ** Temperature Sensor (DS18B20) **
const int oneWireBus = 35;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// ** pH Sensor **
#define pH_PIN 34
float voltageOffset = 0.0;  // Adjust as per calibration
float pHScale = 3.5;        // Adjust scaling if needed

// Function for TDS calculation
float calculateTDS(int analogValue) {
  float voltage = analogValue * (VREF / ADC_RES);
  float tdsValue = (voltage / VREF) * 1000;
  return tdsValue;
}

// Function for pH calculation
float calculatePH(int analogValue) {
  float voltage = (analogValue / ADC_RES) * 3.3;
  float pHValue = (voltage * pHScale) + voltageOffset;
  return pHValue;
}

// Function for gamma correction (for RGB sensor)
float applyGammaCorrection(float value) {
  return pow(value, 1.0 / gammaCorrection) * 255.0;
}

void setup() {
  Serial.begin(115200);

  // ** Wi-Fi Setup **
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  // ** Initialize Sensors **
  if (tcs.begin()) {
    Serial.println("TCS34725 sensor found");
  } else {
    Serial.println("TCS34725 not found. Check your wiring.");
    while (1);
  }
  
  sensors.begin();  // Start DS18B20 temperature sensor
}

void loop() {
  // ** RGB Sensor Data (TCS34725) **
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  uint32_t sum = r + g + b;
  float red = (r * 255.0) / sum;
  float green = (g * 255.0) / sum;
  float blue = (b * 255.0) / sum;
  red = applyGammaCorrection(red / whiteBalance[0]);
  green = applyGammaCorrection(green / whiteBalance[1]);
  blue = applyGammaCorrection(blue / whiteBalance[2]);

  // ** TDS Sensor Data **
  int tdsAnalogValue = analogRead(TDS_PIN);
  float tdsValue = calculateTDS(tdsAnalogValue);
  
  // ** Temperature Sensor Data **
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);

  // ** pH Sensor Data **
  int pHAnalogValue = analogRead(pH_PIN);
  float pHValue = calculatePH(pHAnalogValue);

  // ** Send Data to ThingSpeak **
  ThingSpeak.setField(1, (float)tdsValue);  // TDS value
  ThingSpeak.setField(2, temperatureC);     // Temperature in Celsius
  ThingSpeak.setField(3, pHValue);          // pH value
  ThingSpeak.setField(4, red);              // Red value from RGB sensor
  ThingSpeak.setField(5, green);            // Green value from RGB sensor
  ThingSpeak.setField(6, blue);             // Blue value from RGB sensor

  int responseCode = ThingSpeak.writeFields(channelID, apiKey);
  if (responseCode == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.print("Error sending data: ");
    Serial.println(responseCode);
  }

  delay(20000);  // Wait 20 seconds between updates
}
