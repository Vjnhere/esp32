//code for prabhakaran's project

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// DHT11 Pin and Type
#define DHTPIN 15  // GPIO pin for DHT11 sensor
#define DHTTYPE DHT11

// HW-827 Sensor Pin (Analog)
#define PROXIMITY_PIN 35  // GPIO pin for HW-827 sensor (analog)

// Buzzer Pin
#define BUZZER_PIN 33

// DHT object
DHT dht(DHTPIN, DHTTYPE);

// Variables for pulse rate calculation
unsigned long lastReadTime = 0;
unsigned long lastPulseTime = 0;
unsigned long lastPrintTime = 0;  // Time when HW-827 readings were last printed
int pulseCount = 0;
float pulseRate = 0;

// Customizable threshold and debounce time
const int pulseThreshold = 500;  // Adjust based on your sensor's output range
const unsigned long debounceTime = 100;  // Debounce time in milliseconds
const unsigned long printInterval = 5000;  // Interval for printing HW-827 sensor data (5 seconds)

void setup() {
  Serial.begin(115200);
  
  // Initialize DHT11
  dht.begin();
  Serial.println("DHT11 Sensor Test");
  
  // Initialize HW-827 pin as input
  pinMode(PROXIMITY_PIN, INPUT);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read DHT11 every 5 seconds
  if (currentTime - lastReadTime >= 5000) {
    lastReadTime = currentTime;
    
    // Read temperature from DHT11
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();  // Still keeping humidity but can ignore printing it if needed
    
    if (isnan(temp) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      // Print temperature with condition
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" °C ");
      if (temp > 35) {
        Serial.println("- temp bad");
        digitalWrite(BUZZER_PIN, HIGH);
      } else {
        Serial.println("- temp good");
      }
    }
  }
  
  // Read HW-827 sensor (analog)
  int sensorValue = analogRead(PROXIMITY_PIN);
  
  // Pulse detection logic
  static int lastSensorValue = 0;
  
  if (sensorValue > pulseThreshold && lastSensorValue <= pulseThreshold) {
    // Detected a pulse (crossing the threshold)
    if (currentTime - lastPulseTime > debounceTime) {
      pulseCount++;
      lastPulseTime = currentTime;
    }
  }
  
  lastSensorValue = sensorValue;
  
  // Print HW-827 sensor readings every 5 seconds
  if (currentTime - lastPrintTime >= printInterval) {
    lastPrintTime = currentTime;
    
    // Calculate pulse rate
    pulseRate = (pulseCount / (printInterval / 1000.0));  // Pulse rate in pulses per second
    pulseCount = 0;
    
    // Print HW-827 analog reading and pulse rate with condition
    Serial.print("HW-827 Sensor Value: ");
    Serial.print(sensorValue);
    if (sensorValue > 45) {
      Serial.print(" pulse - bad");
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      Serial.print(" pulse - good");
    }
    
    Serial.print(" - Pulse Rate: ");
    Serial.print(pulseRate);
    Serial.println(" pulses per second");
  }
}
