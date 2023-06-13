#include <Wire.h>
#include <HardwareSerial.h>

const byte i2cAddress = 0x01; // Change the address for each Leonardo board
const int baudRate = 115200;

void setup() {
  Serial.begin(baudRate);
  Serial1.begin(baudRate);

  // Configure the ADS1110
  Wire.beginTransmission(0x48);
  Wire.write(0x85); // Config register address
  Wire.write(0x83); // Single-ended measurement on AIN0, gain = 2/3, mode = continuous conversion, data rate = 860 SPS
  Wire.endTransmission();
}

void loop() {
  // Check if a read request is received from the master
  if (Serial1.available() && Serial1.read() == 'R') {
    // Start a conversion
    Wire.beginTransmission(0x48);
    Wire.write(0x80); // Conversion register address
    Wire.endTransmission();
    delayMicroseconds(15); // Wait for the conversion to finish

    // Read the conversion result (MSB first)
    Wire.requestFrom(0x48, 2);
    byte msb = Wire.read();
    byte lsb = Wire.read();
    int result = (msb << 8) | lsb;

    // Calculate the voltage
    float voltage = static_cast<float>(result) * 0.1875 / 1000.0;

    // Send the measurement message through UART to the master
    Serial1.write(0xAA);         // Start byte
    Serial1.write(i2cAddress);   // Slave ID
    Serial1.write(msb);          // MSB
    Serial1.write(lsb);          // LSB
    byte checksum = 0xAA ^ i2cAddress ^ msb ^ lsb;
    Serial1.write(checksum);     // Checksum
    Serial1.write(0x55);         // End byte

    // Print debug information in the serial monitor
    Serial.print("Voltage sent to Master: ");
    Serial.print(voltage, 4);
    Serial.println(" V");
  }
}
