#include <Wire.h>

const int ADS1110_ADDRESS = 0x48; // I2C address of the ADS1110
const int mosfetPin = 3; // Adjust this to the correct pin for MOSFET control
const int uartSpeed = 1000000; // Speed of UART communication

void setup() {
  Wire.begin();
  Serial1.begin(uartSpeed);
  pinMode(mosfetPin, OUTPUT);
  digitalWrite(mosfetPin, LOW); // Initialize MOSFET as off

  // Configure the ADS1110 for continuous conversion mode
  Wire.beginTransmission(ADS1110_ADDRESS);
  Wire.write(0x85); // Config register address
  Wire.write(0x83); // Single-ended measurement on AIN0, gain = 2/3, mode = continuous conversion, data rate = 860 SPS
  Wire.endTransmission();
}

void loop() {
  // Main Loop
  if (Serial1.available()) {
    // Receive UART message
    int uartValue = Serial1.read();

    // Start a conversion and read the voltage
    float voltage = readVoltage();

    // Compare voltage with UART value (assuming uartValue is in the same scale as voltage)
    if (voltage > uartValue) {
      // Close MOSFET
      digitalWrite(mosfetPin, HIGH);

      // Monitoring Loop
      do {
        // Monitor voltage
        voltage = readVoltage();

        if (voltage <= uartValue) {
          // Open MOSFET
          digitalWrite(mosfetPin, LOW);
          break; // Exit monitoring loop
        }
      } while (true);

    } else {
      // ADC value is less than or equal to UART value, idle
      digitalWrite(mosfetPin, LOW); // Ensure MOSFET is off
    }
  }
}

float readVoltage() {
  // Start a conversion
  Wire.beginTransmission(ADS1110_ADDRESS);
  Wire.write(0x80); // Conversion register address
  Wire.endTransmission();

  // Wait for the conversion to finish
  delay(15);

  // Read the conversion result (MSB first)
  Wire.requestFrom(ADS1110_ADDRESS, 2);
  byte msb = Wire.read();
  byte lsb = Wire.read();
  int result = (msb << 8) | lsb;

  // Calculate voltage
  float voltage = result * 0.1875 / 1000; // 16-bit result * LSB value * Vref / gain / 1000
  return voltage; // Return the voltage in volts
}

