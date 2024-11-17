#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>  // Include the Wi-Fi library

// RGB sensor initialization
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

// Define the pins for the TDS, pH, DS18B20 sensors, and Relay
#define TDS_PIN 32  // TDS sensor connected to GPIO 32
#define pH_PIN 34   // pH sensor connected to GPIO 34
#define RELAY_PIN 12  // Relay connected to GPIO 12
const int oneWireBus = 35; // DS18B20 temperature sensor connected to GPIO 35

// Setup a oneWire instance to communicate with any OneWire devices (DS18B20)
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

// Timer variables for relay control
unsigned long previousMillis = 0;
const unsigned long interval = 120000;  // 2 minutes (120,000 ms)

// Calibration values for pH sensor (adjust based on your specific sensor)
float voltageOffset = 0.0;  // Adjust if your pH sensor has an offset
float pHScale = 3.5;        // Adjust the scaling factor if needed

// Initialize entry_id to count loops
int entry_id = 1;

// Wi-Fi Credentials
const char* ssid = "vjnhere";          // Replace with your Wi-Fi SSID
const char* password = "Vijayan004";  // Replace with your Wi-Fi password

// ThingSpeak Configuration
const char* server = "api.thingspeak.com";
const char* apiKey = "86A7WERB9IQ91N71"; // Replace with your ThingSpeak Write API Key

void setup() {
  Serial.begin(115200);
  
  // Initialize Wi-Fi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize TCS34725 RGB sensor
  if (tcs.begin()) {
    Serial.println("Hydroponic System found");
  } else {
    Serial.println("No Hydroponic System found ... check your connections");
    while (1);
  }
  
  // Set the pin modes for the TDS, pH sensors, and relay
  pinMode(TDS_PIN, INPUT);
  pinMode(pH_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Start the DS18B20 temperature sensor
  sensors.begin();
  
  // Ensure the relay is initially off
  digitalWrite(RELAY_PIN, LOW);
}

void loop() {
  // Get the current time
  unsigned long currentMillis = millis();
  
  // Check if 2 minutes have passed to toggle the relay
  if (currentMillis - previousMillis >= interval) {
    // Toggle the relay
    if (digitalRead(RELAY_PIN) == LOW) {
      digitalWrite(RELAY_PIN, HIGH);  // Turn the relay on
      Serial.println("Relay OFF");
    } else {
      digitalWrite(RELAY_PIN, LOW);   // Turn the relay off
      Serial.println("Relay ON");
    }
    // Save the last toggle time
    previousMillis = currentMillis;
  }

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

  // Calculate Color Temperature (in Kelvin) - simple average for demonstration
  float colorTemp = (red + green + blue) / 3.0;

  // TDS Sensor Reading
  int tdsValue = analogRead(TDS_PIN);

  // pH Sensor Reading
  int pHAnalogValue = analogRead(pH_PIN);
  float pHVoltage = (pHAnalogValue / 4095.0) * 3.3;
  float pHValue = (pHVoltage * pHScale) + voltageOffset;

  // DS18B20 Temperature Sensor Reading
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);

  // Print all sensor values along with entry_id
  Serial.print("Entry ID: "); Serial.print(entry_id);
  Serial.print(", R: "); Serial.print(red);
  Serial.print(", G: "); Serial.print(green);
  Serial.print(", B: "); Serial.print(blue);
  Serial.print(", Clear: "); Serial.print(c);
  Serial.print(", Color Temp: "); Serial.print(colorTemp);
  Serial.print(", TDS Value: "); Serial.print(tdsValue);
  Serial.print(", pH Value: "); Serial.print(pHValue);
  Serial.print(", Temperature: "); Serial.print(temperatureC); Serial.println(" ºC");

  // Send data to ThingSpeak
  sendToThingSpeak(red, green, blue, tdsValue, pHValue, temperatureC, entry_id, c);

  // Increment the entry_id for the next loop
  entry_id++;

  delay(20000);  // Wait 20 seconds before the next reading 
}

void sendToThingSpeak(float red, float green, float blue, int tdsValue, float pHValue, float temperatureC, int entry_id, float c) {
  WiFiClient client;
  if (client.connect(server, 80)) {
    String url = String("/update?api_key=") + apiKey
                + "&field1=" + String(red)
                + "&field2=" + String(green)
                + "&field3=" + String(blue)
                + "&field4=" + String(c)
                + "&field5=" + String(tdsValue)
                + "&field6=" + String(pHValue)
                + "&field7=" + String(temperatureC);

    // Send HTTP GET request
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");
    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.println("Connection to ThingSpeak failed");
  }
  client.stop(); // Close the connection
}
