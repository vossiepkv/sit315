

// Linbaries that are included
#include <Arduino.h>

// Define constants and variables
const int ledPin = 13; // LED connected to digital pin 13
const int potPin = A4; // Potentiometer connected to analog pin A4

void setup() {
  // Initialize pin modes
  pinMode(ledPin, OUTPUT); // Set pin as output
  pinMode(potPin, INPUT); // Set potentiometer pin as input
  Serial.begin(9600); 
}

void loop() {
  // Read potentiometer value
  int potValue = analogRead(potPin);
  
  // Map potentiometer value to frequency range (1 to 10 Hz)
  double frequency = map(potValue, 0, 1023, 1, 10);

  // Start timer with the calculated frequency
  startTimer(frequency);

  // Blink LED every 2 seconds
  digitalWrite(ledPin, HIGH); // Turn the LED on
  delay(2000); // Wait for 2 seconds
  digitalWrite(ledPin, LOW); // Turn the LED off
  delay(2000); 
}

void startTimer(double timerFrequency) {
  // Calculate timer period
  double period = 1.0 / timerFrequency;
  
  // Calculate OCR1A value
  int ocrValue = (int)(16000000 * period / 1024) - 1;

  // Set timer compare register
  OCR1A = ocrValue;

  // Configure timer control registers
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode, prescaler 1024
  
  // Enable timer compare interrupt
  TIMSK1 = (1 << OCIE1A);
}
