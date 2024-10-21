#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <OneWire.h>
#include <DallasTemperature.h>

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

void setup() {
  Serial.begin(115200);
  
  // Initialize TCS34725 RGB sensor
  if (tcs.begin()) {
    Serial.println("TCS34725 found");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
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
      Serial.println("Relay ON");
    } else {
      digitalWrite(RELAY_PIN, LOW);   // Turn the relay off
      Serial.println("Relay OFF");
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
  Serial.print("Entry ID: "); Serial.println(entry_id);
  Serial.print("RGB Values -> R: "); Serial.print(red);
  Serial.print(", G: "); Serial.print(green);
  Serial.print(", B: "); Serial.print(blue);
  Serial.print(", Clear: "); Serial.print(c);
  Serial.print(", Color Temp: "); Serial.print(colorTemp);
  Serial.print(", TDS Value: "); Serial.print(tdsValue);
  Serial.print(", pH Value: "); Serial.print(pHValue);
  Serial.print(", Temperature: "); Serial.print(temperatureC); Serial.println(" ºC");

  // Increment the entry_id for the next loop
  entry_id++;

  delay(20000);  // Wait 20 second before next reading 
} 
