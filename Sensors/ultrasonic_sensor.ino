#define TRIG_PIN 5
#define ECHO_PIN 18

void setup() {
  Serial.begin(115200);  // Start the serial communication
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  long duration, distance;

  // Clear the trigPin condition
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Set the trigPin HIGH for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH, 30000); // Set a timeout of 30000 microseconds

  // Check if a pulse was received
  if (duration == 0) {
    Serial.println("No pulse received, check wiring or sensor placement.");
  } else {
    // Calculate the distance in centimeters
    distance = duration * 0.034 / 2;

    Serial.print("Duration: ");
    Serial.print(duration);
    Serial.print(" us, Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  delay(500);  // Delay between measurements
}
