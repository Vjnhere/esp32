// Define analog pins for MQ3, MQ6, MQ7 sensors
#define MQ3_ANALOG_PIN 34
#define MQ6_ANALOG_PIN 35
#define MQ7_ANALOG_PIN 36

// Define digital pins for MQ3, MQ6, MQ7 sensors
#define MQ3_DIGITAL_PIN 15
#define MQ6_DIGITAL_PIN 2
#define MQ7_DIGITAL_PIN 4

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Set digital pins as input
  pinMode(MQ3_DIGITAL_PIN, INPUT);
  pinMode(MQ6_DIGITAL_PIN, INPUT);
  pinMode(MQ7_DIGITAL_PIN, INPUT);
}

void loop() {
  // Read analog values
  int mq3_analog = analogRead(MQ3_ANALOG_PIN);
  int mq6_analog = analogRead(MQ6_ANALOG_PIN);
  int mq7_analog = analogRead(MQ7_ANALOG_PIN);
  
  // Read digital values
  int mq3_digital = digitalRead(MQ3_DIGITAL_PIN);
  int mq6_digital = digitalRead(MQ6_DIGITAL_PIN);
  int mq7_digital = digitalRead(MQ7_DIGITAL_PIN);
  
  // Print readings
  Serial.println("MQ3 Sensor Readings:");
  Serial.print("Analog: "); Serial.println(mq3_analog);
  Serial.print("Digital: "); Serial.println(mq3_digital);
  
  Serial.println("MQ6 Sensor Readings:");
  Serial.print("Analog: "); Serial.println(mq6_analog);
  Serial.print("Digital: "); Serial.println(mq6_digital);
  
  Serial.println("MQ7 Sensor Readings:");
  Serial.print("Analog: "); Serial.println(mq7_analog);
  Serial.print("Digital: "); Serial.println(mq7_digital);
  
  Serial.println("----------------------------");

  // Wait 30 seconds before the next reading
  delay(30000);
}
