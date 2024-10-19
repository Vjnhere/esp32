#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ThingSpeak.h>
#include "Adafruit_TCS34725.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Replace with your network credentials
const char* ssid = "vjnhere";       // Your WiFi SSID
const char* password = "Vijayan004"; // Your WiFi password

// ThingSpeak settings
unsigned long channelID = 2651888;  // Replace with your ThingSpeak channel ID
const char* apiKey = "8ZYZW3MKKJ8CJTVM"; // Replace with your ThingSpeak Write API Key

WiFiClient client;  // Create a WiFi client instance

// RGB sensor initialization
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

// TDS sensor connected to GPIO 32 (ADC1_4)
#define TDS_PIN 32

// DS18B20 temperature sensor setup
const int oneWireBus = 35;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

// pH sensor connected to GPIO 34
#define pH_PIN 34

// Calibration values for pH sensor
float voltageOffset = 0.0;  
float pHScale = 3.5;        

void setup() {
  Serial.begin(115200);
  
  // Initialize TCS34725 sensor
  if (tcs.begin()) {
    Serial.println("TCS34725 found");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize ThingSpeak with WiFi client
  ThingSpeak.begin(client);

  // Start the DS18B20 sensor
  sensors.begin();
}

void loop() {
  // RGB Sensor Readings
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  uint32_t sum = r + g + b;
  float red, green, blue;
  
  if (sum > 0) {
    red = (r * 255.0) / sum;
    green = (g * 255.0) / sum;
    blue = (b * 255.0) / sum;
  } else {
    red = green = blue = 0;
  }

  // Calculate Color Temperature (in Kelvin)
  float colorTemp = (red + green + blue) / 3.0; // Simple average, adjust for actual calculation if needed

  Serial.print("RGB Values -> Red: "); Serial.print(red);
  Serial.print(", Green: "); Serial.print(green);
  Serial.print(", Blue: "); Serial.print(blue);
  Serial.print(", Color Temp: "); Serial.println(colorTemp);

  // TDS Sensor Readings
  int tdsAnalogValue = analogRead(TDS_PIN);
  float tdsValue = (tdsAnalogValue / 4095.0) * 3.3 * 1000;  // Simplified TDS calculation
  Serial.print("TDS Value: "); Serial.println(tdsValue);

  // Temperature Sensor Readings
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.print("Temperature: "); Serial.print(temperatureC); Serial.println("ºC");

  // pH Sensor Readings
  int analogValue = analogRead(pH_PIN);
  float voltage = (analogValue / 4095.0) * 3.3;
  float pHValue = (voltage * pHScale) + voltageOffset;
  Serial.print("pH Value: "); Serial.println(pHValue);

  // Update ThingSpeak
  ThingSpeak.setField(1, tdsValue);         // Send TDS value to Field 1
  ThingSpeak.setField(2, temperatureC);     // Send Temperature to Field 2
  ThingSpeak.setField(3, pHValue);          // Send pH value to Field 3
  ThingSpeak.setField(4, red);              // Send Red value to Field 4
  ThingSpeak.setField(5, green);            // Send Green value to Field 5
  ThingSpeak.setField(6, blue);             // Send Blue value to Field 6
  ThingSpeak.setField(7, colorTemp);        // Send Color Temperature to Field 7

  int responseCode = ThingSpeak.writeFields(channelID, apiKey); 

  if (responseCode == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.print("Error sending data: ");
    Serial.println(responseCode);
  }

  delay(20000);  // Wait 20 seconds before next reading
}
