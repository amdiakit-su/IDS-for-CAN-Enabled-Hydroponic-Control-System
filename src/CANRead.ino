/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_CAN.h>
#include <CanUtil.h>
#include <R7FA4M1_CAN.h>
// #include <R7FA6M5_CAN.h>
#include <SyncCanMsgRingbuffer.h>

/**************************************************************************************
* CONSTANTS
****************************************************************************** ********/
const int relayPin = 4;
int cnt = 0;

// CAN ID Assignments
static const uint32_t CAN_ID = 0x20; // CAN ID for DHT11 (And all data)
static const uint32_t CAN_ID_TDS = 0x21;           // CAN ID for TDS Sensor
static const uint32_t CAN_ID_TEMP = 0x22;          // CAN Water Temp Sensor

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/
void setup()
{
  Serial.begin(115200);
  while (!Serial) { }

  pinMode(relayPin, OUTPUT);
  Serial.println("Water on");

  // if (!CAN.begin(CanBitRate::BR_250k))
  // {
  //   Serial.println("CAN.begin(...) failed.");
  //   for (;;) {}  // Infinite loop to halt further execution
  // }
}

void loop()
{
  digitalWrite(relayPin, HIGH);


  if (CAN.available())
  {
    CanMsg const msg = CAN.read();
    Serial.print("New Incoming Data ..count: ");
    cnt++;
    Serial.print(cnt);
    Serial.println(" ");

    //Check all data

    if (msg.id == CAN_ID && sizeof(msg.data) >= 8) {
      // Extract individual sensor values from the message data array
      int humidity = msg.data[0];     // Humidity (DHT11)
      int airTemp = msg.data[1];      // Air temperature (DHT11)
      int waterTemp = msg.data[2];    // Water temperature (DS18B20)
      
      // Extract TDS value as a 16-bit integer (little-endian format)
      int tdsValue = (msg.data[5] << 8) | msg.data[4]; 

      // Print the extracted values
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.print(" %");

      Serial.print(", Air Temp: ");
      Serial.print(airTemp);
      Serial.println(" C");

      Serial.print("Water Temp: ");
      Serial.print(waterTemp);
      Serial.println(" C");

      Serial.print("TDS Value: ");
      Serial.print(tdsValue);
      Serial.println(" ppm");

    }


    if (msg.id == CAN_ID_HUMIDITY_TEMP)
    {
      if (sizeof(msg.data) >= 8)  // Expected size of the array
      {
        int humidity = msg.data[2];    // Humidity as integer
        int temperature = msg.data[3]; // Temperature as integer
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.print(" %");

        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" C");
        Serial.println(" ");
      }
    }
    Check for CAN ID TDS (TDS Sensor)
    else if (msg.id == CAN_ID_TDS)
    {
      if (sizeof(msg.data) == 4)  // Expected size of the array
      {
        int tdsValue;
        memcpy(&tdsValue, msg.data, sizeof(tdsValue));
        Serial.print("TDS Value: ");
        Serial.print(tdsValue);
        Serial.println(" ppm");
      }
    }
    // Check for CAN ID Water Temperature Sensor
    else if (msg.id == CAN_ID_TEMP)
    {
      if (sizeof(msg.data) >= 3)  // Expected size of the array
      {
       int waterTemp = msg.data[2]; // Extract the temperature as an integer
        Serial.print("Water Temperature: ");
        Serial.print(waterTemp); // Directly printing without conversion
        Serial.println(" C");
      }
    }
    If the CAN ID is unknown or unexpected
    else
    {
      Serial.println("Unknown or unexpected CAN ID.");
    }
  
}

