#include <Wire.h>

const int buttonTogglePin = 10; 
const int buttonNextPin = 11;   
const int pwmOutputPin = 9;     
const int clkButtonPin = 5;

int clkOutputPin = 6;
bool is16BitMode = false;
int currentBit = 0;
int bits[8] = {0};     

int clk = 60;
bool clkToggleState = false;
bool buttonToggleState = false;
bool lastButtonToggleState = false;
bool buttonNextState = false;
bool lastButtonNextState = false;

void setup() {
  pinMode(buttonTogglePin, INPUT_PULLUP);
  pinMode(buttonNextPin, INPUT_PULLUP);
  pinMode(clkButtonPin, INPUT_PULLUP);
  pinMode(pwmOutputPin, OUTPUT);
  pinMode(clkOutputPin,OUTPUT);
  Serial.begin(9600); // Initialize serial communication for console output

  resetBits();
}

void loop() {
  buttonToggleState = digitalRead(buttonTogglePin);
  buttonNextState = digitalRead(buttonNextPin);

  //clkToggleState = digitalRead(clkButtonPin);
  
  
  // Toggle mode between 8-bit and 16-bit when both buttons are pressed
  if (buttonToggleState == LOW && buttonNextState == LOW) {
    is16BitMode = !is16BitMode; 
    resetBits();
    delay(500); 
  }

  // if(clkToggleState == LOW){
  //   clk = (clk == 60) ? 25:60;
  //   delay(10);
  // }

  if (buttonToggleState == LOW && lastButtonToggleState == HIGH) {
    bits[currentBit] = !bits[currentBit];
    updateDisplay();
  }

  if (buttonNextState == LOW && lastButtonNextState == HIGH) {
    int maxBits = is16BitMode ? 16 : 8;

    if (currentBit == maxBits - 1) {
      startTransmission(maxBits); // Start continuous data generation 
    } else {
      currentBit++;
    }
    updateDisplay();
  }

  lastButtonToggleState = buttonToggleState;
  lastButtonNextState = buttonNextState;
}

void updateDisplay() {
  // No need to clear the serial monitor, just print output to the console
  Serial.println(); // Adding a newline for clarity
  int maxBits = 8; // You can change this to 16 if using 16-bit mode
  for (int i = 0; i < maxBits; i++) {
    if (i == currentBit) {
      Serial.print("[");
    }
    Serial.print(bits[i]);
    if (i == currentBit) {
      Serial.print("]");
    }
    Serial.print(" ");
  }
  Serial.println(); // Newline after displaying the bit pattern
}

void startTransmission(int maxBits) {
  Serial.println("Streaming data...");

  int clkData = 1;

  while (true) { // Infinite loop for continuous streaming
    for (int i = 0; i < maxBits; i++) {
      digitalWrite(clkOutputPin, clkData);
      clkData = (clkData == 1)?0:1;
      digitalWrite(pwmOutputPin, bits[i]);  // Output bit (1 or 0)
      delayMicroseconds(clk*2); // Adjust for timing control
    }

    

    // Check if user wants to reset (hold both buttons to stop)
    if (digitalRead(buttonTogglePin) == LOW && digitalRead(buttonNextPin) == LOW) {
      Serial.println("Stopped!");
      digitalWrite(pwmOutputPin, LOW);  // Ensure the output is LOW when stopping
      resetBits();
      return; // Exit the loop and return to normal operation
    }
  }
}

void resetBits() {
  for (int i = 0; i < 8; i++) {  // Corrected to 8 bits
    bits[i] = 0;
  }
  currentBit = 0;
  updateDisplay();
}
