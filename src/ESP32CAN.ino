#include <CAN.h>

unsigned long lastSendTime = 0;  // Last send time
const int sendInterval = 1000;   // Send interval in milliseconds

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

void loop() {

  int packetSize = CAN.parsePacket();

  if (packetSize > 0) {  // Check if a packet has been received
    Serial.println("Intercepted Data ...");
    Serial.print("Received frame with id 0x");
    Serial.print(CAN.packetId(), HEX);  // Print the packet ID in hexadecimal
    Serial.print(" and length ");
    Serial.println(packetSize); 

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

     if (!CAN.packetRtr()) { // Only handle the packet if it's not a RTR (remote transmission request) packet
      uint8_t data[8]; // Expected data length
      int index = 0;

      // Read each byte of the received CAN message into the `data` array
      while (CAN.available() && index < sizeof(data)) {
        data[index++] = CAN.read();
      }

      // Extract individual sensor values using `memcpy`
      uint8_t humidity = 0, airTemp = 0, waterTemp = 0;
      uint16_t tdsValue = 0;

      // Extract and print values
      memcpy(&humidity, &data[0], 1);
      memcpy(&airTemp, &data[1], 1);
      memcpy(&waterTemp, &data[2], 1);
      memcpy(&tdsValue, &data[4], 2); // Extract 2 bytes for the TDS value

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println("%");

      Serial.print("Air Temperature: ");
      Serial.print(airTemp);
      Serial.println("C");

      Serial.print("Water Temperature: ");
      Serial.print(waterTemp);
      Serial.println("C");

      Serial.print("TDS Value: ");
      Serial.print(tdsValue);
      Serial.println(" ppm");
    }

    Serial.println();
  }
  delay(7000);
  // Send a high priority message every second
  // unsigned long currentTime = millis();
  // if (currentTime - lastSendTime >= sendInterval) {
  //   lastSendTime = currentTime;
  //   sendHighPriorityMessage();
  // }
}

// void sendHighPriorityMessage() {
//   CAN.beginPacket(0x10);  // Lower ID for higher priority
//   CAN.write('H');         // Example data
//   CAN.write('i');
//   CAN.write('!');
//   CAN.endPacket();

//   Serial.println("High priority message sent");
// }