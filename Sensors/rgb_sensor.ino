#include <Wire.h>

// Define the I2C address of the TCS43725 sensor
#define TCS43725_ADDRESS 0x29

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize I2C communication
  Wire.begin(21, 22); // SDA, SCL

  // Initialize the sensor
  initSensor();
}

void loop() {
  // Read color data from the sensor
  uint16_t r, g, b;
  readColorData(r, g, b);

  // Print the color data
  Serial.print("R: ");
  Serial.print(r);
  Serial.print(" G: ");
  Serial.print(g);
  Serial.print(" B: ");
  Serial.println(b);

  // Delay before the next reading
  delay(1000);
}

void initSensor() {
  // Initialize the TCS43725 sensor with its configuration registers
  Wire.beginTransmission(TCS43725_ADDRESS);
  Wire.write(0x00); // Example register address for initialization
  Wire.write(0x01); // Example initialization value
  Wire.endTransmission();
}

void readColorData(uint16_t &r, uint16_t &g, uint16_t &b) {
  // Read RGB color data from the sensor
  Wire.beginTransmission(TCS43725_ADDRESS);
  Wire.write(0x01); // Example register address to read RGB data
  Wire.endTransmission();
  
  Wire.requestFrom(TCS43725_ADDRESS, 6); // Request 6 bytes of data
  
  if (Wire.available() == 6) {
    r = Wire.read() | (Wire.read() << 8); // Read Red data
    g = Wire.read() | (Wire.read() << 8); // Read Green data
    b = Wire.read() | (Wire.read() << 8); // Read Blue data
  }
}
