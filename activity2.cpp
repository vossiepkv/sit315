// Pin number connected to the button
const uint8_t BTN_PIN = 2;
// Pin number connected to the LED
const uint8_t LED_PIN = 13;

// Variable to store the previous button state
uint8_t buttonPrevState = LOW;
// Variable to store the LED state
uint8_t ledState = LOW;

void setup()
{
  // Set the button pin as an input with an internal pull-up resistor
  pinMode(BTN_PIN, INPUT_PULLUP);
  // Set the LED pin as an output
  pinMode(LED_PIN, OUTPUT);
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);
}

void loop()
{
  // Read the current button state
  uint8_t buttonState = digitalRead(BTN_PIN);

  // Print the current button state, previous button state, and LED state to the serial monitor
  Serial.print(buttonState);
  Serial.print(buttonPrevState);
  Serial.print(ledState);
  Serial.println("");

  // If the button state has changed since the last loop iteration
  if (buttonState != buttonPrevState)
  {
    // Toggle the LED state
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }

  // Update the previous button state to the current state
  buttonPrevState = buttonState;

  // Introduce a delay of 500 milliseconds
  delay(500);
}
