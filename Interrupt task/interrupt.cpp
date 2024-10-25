const int tempPin = A0; // TMP36 Vout pin connected to A0
const int ledPin = 13; // LED connected to pin 13
const int interruptPin = 2; // Interrupt pin

volatile bool tempHigh = false; // Flag to check temperature

void setup() {
  pinMode(ledPin, OUTPUT); // Set LED pin as output
  pinMode(interruptPin, INPUT_PULLUP); // Set interrupt pin as input with pullup resistor
  Serial.begin(9600); // Initialize serial communication

  // Attach interrupt to pin 2, trigger on FALLING edge
  attachInterrupt(digitalPinToInterrupt(interruptPin), checkTemperature, FALLING);
}

void loop() {
  // Main loop does nothing, all work done in ISR
}

void checkTemperature() {
  int tempReading = analogRead(tempPin); // Read temperature sensor
  float voltage = tempReading * (5.0 / 1023.0); // Convert analog reading to voltage
  float temperatureC = (voltage - 0.5) * 100.0; // Convert voltage to temperature in Celsius

  // Print temperature to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  // Check if temperature is above threshold
  if (temperatureC > 25.0) {
    tempHigh = true;
    digitalWrite(ledPin, HIGH); // Turn LED on
  } else {
    tempHigh = false;
    digitalWrite(ledPin, LOW); // Turn LED off
  }
}

