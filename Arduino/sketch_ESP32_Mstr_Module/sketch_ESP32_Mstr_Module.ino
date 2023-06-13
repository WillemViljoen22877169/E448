#include <HardwareSerial.h>

const byte numSlaves = 4;
const int baudRate = 115200;

void setup() {
  Serial.begin(baudRate);
  // Initialize the UART communication for each slave
  for (byte i = 0; i < numSlaves; i++) {
    Serial1.begin(baudRate);
  }
}

void loop() {
  // Start a communication cycle
  for (byte i = 0; i < numSlaves; i++) {
    // Request the voltage value from the current slave
    Serial.print("Requesting voltage from Slave ");
    Serial.println(i);
    Serial1.write('R');
    delayMicroseconds(100); // Wait for the slave to respond

    // Read the voltage value from the slave
    if (Serial1.available() >= 7) {
      byte startByte = Serial1.read();
      byte slaveID = Serial1.read();
      byte msb = Serial1.read();
      byte lsb = Serial1.read();
      byte checksum = Serial1.read();
      byte endByte = Serial1.read();

      // Validate the framing and checksum
      if (startByte == 0xAA && endByte == 0x55 && checksum == (startByte ^ slaveID ^ msb ^ lsb)) {
        int result = (msb << 8) | lsb;
        float voltage = static_cast<float>(result) * 0.1875 / 1000.0;

        // Print the voltage value received from the slave
        Serial.print("Received voltage from Slave ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(voltage, 4);
        Serial.println(" V");
      } else {
        // Print an error message if the framing or checksum is invalid
        Serial.println("Invalid framing or checksum");
      }
    } else {
      // Print an error message if the expected number of bytes is not received
      Serial.println("Incomplete message");
    }
  }

  // Delay before starting the next communication cycle
  delayMicroseconds(1000);
}
