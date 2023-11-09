#include <Arduino.h>

// Define the read and write GPIO pins
const int readPins[] = {32, 33, 34, 35, 36};
const int writePins[] = {25, 26, 27, 14, 12};

void setup() {
  // Initialize Serial communication
  Serial.begin(115200);
  // Set the digital read pins
  for(int i = 0; i < 5; i++) {
    pinMode(readPins[i], INPUT);
  }
  // Set the digital write pins
  for(int i = 0; i < 5; i++) {
    pinMode(writePins[i], OUTPUT);
  }
}

// Function to pulse a pin
void pulsePin(int pin, int cycles, long pulseWidth, float dutyCycle, long delayAfter) {
  for(int i = 0; i < cycles; i++) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(pulseWidth * dutyCycle);
    digitalWrite(pin, LOW);
    delayMicroseconds(pulseWidth * (1 - dutyCycle));
  }
  delayMicroseconds(delayAfter);
}

// Function to read pins and print to the Serial plotter
void readAndPlot() {
  for(int i = 0; i < 5; i++) {
    Serial.print("V(rx");
    Serial.print(i);
    Serial.print("): ");
    Serial.print(digitalRead(readPins[i]));
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  // Read and plot the input pins
  readAndPlot();
  // Start the pulse sequence
  pulsePin(writePins[0], 5, 2500, 0.5, 5000);
  pulsePin(writePins[1], 5, 2500, 0.5, 5000);
  pulsePin(writePins[2], 9, 2500, 0.5, 5000);
  // Start V(tx3) on the sixth cycle of V(tx2)
  for(int i = 0; i < 9; i++) {
    if(i == 5) {
      pulsePin(writePins[3], 5, 2500, 0.5, 0);
    }
    digitalWrite(writePins[2], HIGH);
    delayMicroseconds(2500 * 0.5);
    digitalWrite(writePins[2], LOW);
    delayMicroseconds(2500 * (1 - 0.5));
  }
  pulsePin(writePins[4], 5, 2500, 0.5, 0);
}
