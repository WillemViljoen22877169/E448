#include <HardwareSerial.h>

HardwareSerial mySerial(1); // Use UART1
const int currentSensorPin = 4; // GPIO4 connected to ACS770 output
const int relayOnPin = 16;
const int relayOffPin = 17;

unsigned long startTime;
unsigned long lastPulseTime;
unsigned long lastDataTransmitTime;
const unsigned long fiveMinutes = 300000; // 5 minutes in milliseconds
const unsigned long tenMinutes = 600000; // 10 minutes in milliseconds
const unsigned long thirtyMinutes = 1800000; // 30 minutes in milliseconds
const unsigned long fortyMinutes = 2400000; // 40 minutes in milliseconds
const unsigned long dataTransmitInterval = 500; // Data transmit interval in milliseconds

int dataPointCounter = 0;
const int totalDataPoints = 4125; // Total data points to transmit
bool relayState = false; // false = OFF, true = ON
bool transmissionStarted = false;
float lastMeasuredCurrent = 0.0;
String lastReceivedVoltage = "0.00";

void setup() {
  Serial.begin(9600); // Start the serial communication with the computer
  mySerial.begin(9600, SERIAL_8N1, 16, 17); // RX, TX pins (16=RX, 17=TX) at 9600 baud rate
  pinMode(currentSensorPin, INPUT);
  pinMode(relayOnPin, OUTPUT);
  pinMode(relayOffPin, OUTPUT);

  digitalWrite(relayOnPin, LOW);
  digitalWrite(relayOffPin, LOW);

  startTime = millis(); // Save the start time
  lastPulseTime = startTime; // Initialize last pulse time
  lastDataTransmitTime = startTime; // Initialize last data transmit time
  Serial.println("ESP32 UART Voltage Receiver and Current Sensor");
}

void loop() {
  unsigned long currentTime = millis();

  // Measure current and check UART for voltage
  measureAndTransmit();

  // After five minutes from start, begin pulsing sequence
  if (currentTime - startTime > fiveMinutes && !transmissionStarted) {
    pulsePin(relayOffPin);
    relayState = false; // Relay is now OFF
    lastPulseTime = currentTime;
    transmissionStarted = true; // Start data transmission
  }

  // Transmit data every 500 ms after pulsing sequence begins
  if (transmissionStarted && currentTime - lastDataTransmitTime >= dataTransmitInterval && dataPointCounter < totalDataPoints) {
    transmitData(currentTime - lastPulseTime);
    lastDataTransmitTime = currentTime;
  }

  // Stop transmitting after 4125 data points and pulse pin 17
  if (dataPointCounter >= totalDataPoints) {
    pulsePin(relayOffPin); // Pulse pin 17 to open the solid-state relay
    transmissionStarted = false; // Stop data transmission
  }
}

void measureAndTransmit() {
  // Read current sensor value
  int sensorValue = analogRead(currentSensorPin);
  float voltageFromSensor = sensorValue * (3.3 / 4095.0);
  lastMeasuredCurrent = ((voltageFromSensor - 1.65) / (1.65 / 100.0));

  // Check if data is available to read from UART
  if (mySerial.available()) {
    lastReceivedVoltage = mySerial.readStringUntil('\n');
  }
}

void transmitData(unsigned long elapsedTime) {
  // t is the elapsed time in seconds since the pulsing sequence began
  float t = elapsedTime / 1000.0;
  Serial.print(t);
  Serial.print(",");
  Serial.print(lastMeasuredCurrent, 4);
  Serial.print(",");
  Serial.println(lastReceivedVoltage);
  dataPointCounter++;
}

void pulsePin(int pin) {
  digitalWrite(pin, HIGH);
  delay(20);
  digitalWrite(pin, LOW);
}
