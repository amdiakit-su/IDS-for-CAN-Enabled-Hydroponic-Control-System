/**************************************************************************************
 * INCLUDES
 **************************************************************************************/
#include <DHT.h>
#include <Arduino_CAN.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/
DHT dht(2, DHT22); // Initialize the DHT22 sensor on pin 2
const uint32_t CAN_ID_DHT22 = 0x100; // Define a CAN ID for the message

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

  dht.begin(); // Initialize DHT sensor
  Serial.println("CANWrite and DHT22 setup complete.");
}

void loop() {
  float humidity = dht.readHumidity(true);
  float temperature = dht.readTemperature();

  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(1000);
    return;
  }

  // CAN message data (8 bytes)
  uint8_t msg_data[8];
  memcpy(&msg_data[0], &humidity, sizeof(humidity));  // Copy humidity as float
  memcpy(&msg_data[4], &temperature, sizeof(temperature)); // Copy temperature as float

  // Create CAN message
  CanMsg const msg(CanStandardId(CAN_ID_DHT22), sizeof(msg_data), msg_data);

  // Send the CAN message
  if (int const rc = CAN.write(msg); rc < 0) {
    Serial.print("Failed to send CAN message with error code: ");
    Serial.println(rc);
  } else {
    Serial.print("Sent CAN message: ");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %, Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
  }

  delay(2000); // Send every 2 seconds
}
