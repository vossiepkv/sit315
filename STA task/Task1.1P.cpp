const int tempPin = A0; // TMP36 Vout pin connected to A0
const int ledPin = 13; // LED connected to pin 13

void setup() {
  pinMode(ledPin, OUTPUT); // Set LED pin as output
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  int tempReading = analogRead(tempPin); // Read temperature sensor
  float voltage = tempReading * (5.0 / 1023.0); // Convert analog reading to voltage
  float temperatureC = (voltage - 0.5) * 100.0; // Convert voltage to temperature in Celsius
  
  // Print temperature to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  // Turn LED on or off based on temperature
  if (temperatureC > 25.0) { // Example threshold
    digitalWrite(ledPin, HIGH); // Turn LED on
  } else {
    digitalWrite(ledPin, LOW); // Turn LED off
  }

  delay(1000); // Wait for 1 second before repeating
}
