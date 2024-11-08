#include <Wire.h>
#include <DHT.h>
#include <Adafruit_MAX30100.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 4          // DHT11 sensor pin
#define DHTTYPE DHT11
#define MAX30100_IR_PIN 33 // Pin for MAX30100 IR output (connect as needed)
#define MAX30100_RED_PIN 32 // Pin for MAX30100 RED output (connect as needed)
#define PULSE_PIN 34       // Pin for pulse sensor
#define LCD_RS 14          // Pin for LCD RS
#define LCD_EN 27          // Pin for LCD EN
#define LCD_D4 26          // Pin for LCD D4
#define LCD_D5 25          // Pin for LCD D5
#define LCD_D6 17          // Pin for LCD D6
#define LCD_D7 16          // Pin for LCD D7

// WiFi and ThingSpeak setup
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
String apiKey = "your_ThingSpeak_API_KEY";
const char* server = "http://api.thingspeak.com/update";

// Sensor objects
DHT dht(DHTPIN, DHTTYPE);
Adafruit_MAX30100 max30100;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

unsigned long lastUpdate = 0; // Timestamp for 30-second interval

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  dht.begin();
  max30100.begin();
  max30100.setMode(MAX30100_MODE_SPO2_HR);
  
  lcd.begin(16, 2);
  lcd.print("Sensor Init...");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (millis() - lastUpdate >= 30000) {
    lastUpdate = millis();
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    float ir, red;
    int pulse = analogRead(PULSE_PIN);
    
    max30100.update();
    ir = max30100.getIR();
    red = max30100.getRed();
    
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    
    Serial.print("MAX30100 IR: ");
    Serial.print(ir);
    Serial.print(", RED: ");
    Serial.println(red);

    Serial.print("Pulse: ");
    Serial.println(pulse);
    
    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(temperature);
    lcd.print("C ");
    lcd.print("Hum:");
    lcd.print(humidity);
    lcd.setCursor(0, 1);
    lcd.print("Pulse:");
    lcd.print(pulse);
    
    // Send data to ThingSpeak
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String serverPath = server + "?api_key=" + apiKey + "&field1=" + String(temperature) +
                          "&field2=" + String(humidity) + "&field3=" + String(ir) +
                          "&field4=" + String(red) + "&field5=" + String(pulse);
      
      http.begin(serverPath.c_str());
      int httpResponseCode = http.GET();
      
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Data sent to ThingSpeak: " + response);
      } else {
        Serial.println("Error sending data to ThingSpeak");
      }
      
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
  }
}
