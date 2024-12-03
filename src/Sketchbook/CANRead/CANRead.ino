/**************************************************************************************
 * INCLUDES
 **************************************************************************************/
#include <Arduino_CAN.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/
// List of recognized CAN IDs (authorized devices)
const uint32_t AUTHORIZED_IDS[] = {0x100};
const int NUM_AUTHORIZED_IDS = sizeof(AUTHORIZED_IDS) / sizeof(AUTHORIZED_IDS[0]);

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/
void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (!CAN.begin(CanBitRate::BR_250k)) {
    Serial.println("Failed to initialize CAN!");
    while (1); // Stop here if CAN initialization fails
  }

  Serial.println("Monitoring ECU setup complete.");
}

void loop() {
  if (CAN.available()) {
    CanMsg msg = CAN.read();
    bool isAuthorized = false;


    float humidity;
    float temperature;

    // Check if the received ID is in the authorized list
    for (int i = 0; i < NUM_AUTHORIZED_IDS; i++) {
      if (msg.id == AUTHORIZED_IDS[i]) {
        isAuthorized = true;
        // Extract humidity and temperature from the CAN message
        memcpy(&humidity, &msg.data[0], 4); // Copy the first 4 bytes for humidity
        memcpy(&temperature, &msg.data[4], 4); // Copy the next 4 bytes for temperature
        break;
      }
    }

    // Handle the message based on its authorization status
    if (isAuthorized) {
      Serial.print("Authorized message received. ID: 0x");
      Serial.print(msg.id, HEX);
      Serial.println(" - Processing message...");
      Serial.print("Received from ECU ");
      Serial.print(": Humidity: ");
      Serial.print(humidity);
      Serial.print(" %, Temperature: ");
      Serial.print(temperature);
      Serial.println(" °C");

    } else {
      Serial.print("Unauthorized message detected! ID: 0x");
      Serial.print(msg.id, HEX);
      Serial.println(" - Logging intrusion...");
      // Log the intrusion
    }
  }
}