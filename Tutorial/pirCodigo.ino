int PIR_Tutorial = 2;

void setup() {
  // Initialize the serial
  Serial.begin(250000);
  // Set the pin for PIR_Tutorial
  pinMode(PIR_Tutorial, INPUT);
}

void loop() {
  // If the movement is detected, print to serial
  if (digitalRead(PIR_Tutorial) == HIGH) {
    Serial.println("Movimento detectado");
  }
}