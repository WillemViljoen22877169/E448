#include <Wire.h>

const int ADS1110_ADDRESS = 0x48; // I2C address of the ADS1110

void setup() {
  Wire.begin(); // Initialize I2C communication
  Serial1.begin(9600); // Initialize UART communication at 9600 baud rate

  // Configure the ADS1110 for continuous conversion mode
  Wire.beginTransmission(ADS1110_ADDRESS);
  Wire.write(0x85); // Config register address
  Wire.write(0x83); // Single-ended measurement on AIN0, gain = 2/3, mode = continuous conversion, data rate = 860 SPS
  Wire.endTransmission();
}

void loop() {
  float voltage = readVoltage(); // Measure the voltage

  // Transmit the voltage measurement via UART
  Serial1.print("Voltage: ");
  Serial1.print(voltage, 4); // Print voltage with 4 decimal places
  Serial1.println(" V");

  delay(500); // Wait for a half second before taking the next measurement
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

