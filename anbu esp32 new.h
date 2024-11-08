#include <Wire.h>
#include <DHT.h>
#include <PulseOximeter.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Constants and Pins
#define DHTPIN 4          // DHT11 data pin connected to GPIO4
#define DHTTYPE DHT11     // Type of DHT sensor
#define PULSE_SENSOR_PIN 34 // Analog input for the pulse sensor

// LCD Pins
LiquidCrystal lcd(14, 27, 26, 25, 17, 16); // RS, EN, D4, D5, D6, D7

// WiFi and ThingSpeak credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* server = "http://api.thingspeak.com/update";
const char* api_key = "YOUR_API_KEY";

// Sensor objects
DHT dht(DHTPIN, DHTTYPE);
PulseOximeter pox;

// Global variables
float temperature, humidity, heartRate, SpO2;
unsigned long lastUpdateTime = 0;

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
  lcd.clear();

  // Initialize DHT sensor
  dht.begin();

  // Initialize Pulse Oximeter
  if (!pox.begin()) {
    Serial.println("MAX30100 failed to initialize. Check wiring.");
    while (1);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA); // Adjust the IR current

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
}

void loop() {
  // Update sensor readings every 30 seconds
  if (millis() - lastUpdateTime >= 30000) {
    lastUpdateTime = millis();

    // Get DHT sensor readings
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    // Get MAX30100 sensor readings
    pox.update();
    heartRate = pox.getHeartRate();
    SpO2 = pox.getSpO2();

    // Display on Serial Monitor
    Serial.println("DHT11 Sensor Data:");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.println("MAX30100 Sensor Data:");
    Serial.print("Heart Rate: ");
    Serial.print(heartRate);
    Serial.println(" BPM");
    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println(" %");

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temperature);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("HR:");
    lcd.print(heartRate);
    lcd.print(" SpO2:");
    lcd.print(SpO2);

    // Send data to ThingSpeak
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = String(server) + "?api_key=" + api_key +
                   "&field1=" + String(temperature) +
                   "&field2=" + String(humidity) +
                   "&field3=" + String(heartRate) +
                   "&field4=" + String(SpO2);
      http.begin(url);
      int httpResponseCode = http.GET();
      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
  }
}
