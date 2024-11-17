#include <WiFi.h>
#include <HTTPClient.h>
#include <ThingSpeak.h>

// TDS sensor connected to ADC1_4 (GPIO 15)
#define TDS_PIN 32

// WiFi credentials
const char* ssid = "vjnhere";       // Replace with your WiFi network name
const char* password = "Vijayan004"; // Replace with your WiFi network password

// ThingSpeak settings
unsigned long channelID = 2651888;  // Replace with your ThingSpeak channel ID (unsigned long)
const char* apiKey = "8ZYZW3MKKJ8CJTVM"; // Replace with your ThingSpeak Write API Key

WiFiClient client;  // Create a WiFi client instance

// TDS calibration constants
const float VREF = 3.3;  // ESP32 ADC reference voltage (typically 3.3V)
const float ADC_RES = 4095.0;  // ESP32 ADC resolution (12-bit ADC)

// Function to calculate TDS value
float calculateTDS(int analogValue) {
  float voltage = analogValue * (VREF / ADC_RES);  // Convert analog value to voltage
  float tdsValue = (voltage / VREF) * 1000;        // Simplified TDS calculation (adjust based on sensor calibration)
  return tdsValue;
}

void setup() {
  Serial.begin(115200);

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
}

void loop() {
  int tdsAnalogValue = analogRead(TDS_PIN);  // Read TDS sensor analog input from GPIO 15
  float tdsValue = calculateTDS(tdsAnalogValue);  // Calculate TDS value
  Serial.print("Raw Analog Value: ");
  Serial.println(tdsAnalogValue);  // Check if you get a valid analog value (should not be zero)


  Serial.print("TDS Value: ");
  Serial.println(tdsValue);

  // Update ThingSpeak
  ThingSpeak.setField(1, (float)tdsValue);  // Send TDS value to Field 1 of the ThingSpeak channel
  int responseCode = ThingSpeak.writeFields(channelID, apiKey);  // Use writeFields() to send data to ThingSpeak

  if (responseCode == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.print("Error sending data: ");
    Serial.println(responseCode);
  }

  delay(20000);  // Wait 20 seconds before next reading (ThingSpeak allows an update every 15 seconds)
}
