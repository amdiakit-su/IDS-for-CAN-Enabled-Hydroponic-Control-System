/**************************************************************************************
* INCLUDES
**************************************************************************************/
#include <DHT.h>
#include <Arduino_CAN.h>
#include <CanUtil.h>
#include <R7FA4M1_CAN.h>
// #include <R7FA6M5_CAN.h>
#include <SyncCanMsgRingbuffer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
 
/**************************************************************************************
* CONSTANTS
**************************************************************************************/
#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//CAN ID Assignements
static uint32_t const CAN_ID = 0x20; // CAN ID DHT11
static uint32_t const CAN_ID_TDS = 0x21; // CAN ID TDS Sensor
static uint32_t const CAN_ID_TEMP = 0x22; // CAN ID Water Temp Sensor


// DHT11 dht11(2); // DHT11 sensor connected to digital pin 2
DHT dht(2, DHT11);  // initialize the DHT11 sensor


// TDS Sensor Setup
#define TdsSensorPin A1
#define VREF 5.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point
int analogBuffer[SCOUNT]; // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;

/**************************************************************************************
* SETUP/LOOP
**************************************************************************************/
void setup() {
    Serial.begin(115200);
    while (!Serial) { }

    // dht11.setDelay(500); // Optional: Set custom delay between sensor readings
    dht.begin();
    sensors.begin();  // Start the DS18B20 sensor
    pinMode(TdsSensorPin, INPUT);

   

    if (!CAN.begin(CanBitRate::BR_250k)) {
        Serial.println("CAN.begin(...) failed.");
        // while(1); // Stop execution if CAN initialization fails
    }
}

// Function to find the median number in an array
int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;

  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
      bTemp = bTab[i];
      bTab[i] = bTab[i + 1];
      bTab[i + 1] = bTemp;
      }
    }
  }
  return (iFilterLen & 1) ? bTab[(iFilterLen - 1) / 2] : (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;

  // if ((iFilterLen & 1) > 0)
  //   bTemp = bTab[(iFilterLen - 1) / 2];
  // else
  //   bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  // return bTemp;
  }

  float readTDS(float temperature) {
      int copyIndex = 0;
      for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
          analogBuffer[copyIndex] = analogRead(TdsSensorPin);
    }

    int medianValue = getMedianNum(analogBuffer, SCOUNT);
    float voltage = medianValue * (float)VREF / 1024.0;
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
    float compensatedVoltage = voltage / compensationCoefficient;
    return (133.42 * compensatedVoltage * compensatedVoltage * compensatedVoltage - 255.86 * compensatedVoltage * compensatedVoltage + 857.39 * compensatedVoltage) * 0.5;
}

void loop() {

    sensors.requestTemperatures(); // Get temperature readings
    float waterTemp = sensors.getTempCByIndex(0); // Get temperature in Celsius

    float tdsValue = readTDS(waterTemp);

    // int intWaterTemp = sensors.getTempCByIndex(0);
    int h = dht.readHumidity();        // Read humidity in percentage
    int t = dht.readTemperature();     // Read temperature in Celsius

    // Check if any reads failed (DHT11 returns -1 if failed)
    if (h == -1 || t == -1) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }



    // // TDS Message
    // uint8_t tdsData[8] = {0xCA, 0xFE};  // Initialize header
    // int tdsInt = static_cast<int>(tdsValue);
    // memcpy(&tdsData[4], &tdsInt, sizeof(tdsInt));  // Copy TDS value into the remaining bytes
    // CanMsg tdsMsg = {CAN_ID_TDS, sizeof(tdsData), {}};
    // memcpy(tdsMsg.data, tdsData, sizeof(tdsData));

    // Water Temperature Message
    // uint8_t tempData[8] = {0xCA, 0xFE, static_cast<uint8_t>(intWaterTemp)};  // Initialize header
    // memcpy(&tempData[4], &intWaterTemp, sizeof(intWaterTemp));
    // CanMsg temp_msg(CAN_ID_TEMP, sizeof(tempData), tempData);
   

    //DHT11
    // uint8_t msg_data[8] = {0xCA, 0xFE, static_cast<uint8_t>(h), static_cast<uint8_t>(t)};
    // memcpy(&msg_data[4], &h, sizeof(h)); // Copy 2 bytes of humidity
    // memcpy(&msg_data[6], &t, sizeof(t)); // Copy 2 bytes of temperature
    // CanMsg msg(CAN_ID, sizeof(msg_data), msg_data);

    // Prepare the combined CAN message data
    uint8_t canData[8] = {0}; // Initialize with zero for padding
    memcpy(&canData[0], &h, 1);            // 1 byte for humidity
    memcpy(&canData[1], &t, 1);             // 1 byte for air temperature
    int intWaterTemp = static_cast<int>(waterTemp);
    memcpy(&canData[2], &intWaterTemp, 1);        // 1 byte for water temperature
    uint16_t intTdsValue = static_cast<uint16_t>(tdsValue);
    memcpy(&canData[4], &intTdsValue, 2);         // 2 bytes for TDS value

    // Send the CAN message
    CanMsg msg(CAN_ID, sizeof(canData), canData);
    if (int const rc = CAN.write(msg); rc < 0) {
        Serial.print("CAN.write(msg) failed with error code ");
        Serial.println(rc);
        for (;;) {} // Stop execution if CAN message write fails
    }

    // if (int const rc = CAN.write(msg); rc < 0) {
    //     Serial.print("CAN.write(humidityTempMsg) failed with error code");
    //     Serial.println(rc);
    //     for (;;) {}
    // }

    // if (int const rc = CAN.write(tdsMsg); rc < 0) {
    // Serial.print("CAN.write(TDSMsg) failed with error code");
    // Serial.println(rc);
    // for (;;) {}
    // }

    
    // if (int const rc = CAN.write(temp_msg); rc < 0) {
    // Serial.print("CAN.write(TempMsg) failed with error code");
    // Serial.println(rc);
    // for (;;) {}
    // }

    // CAN.write(humidityTempMsg);
    // CAN.write(tdsMsg);
    // CAN.write(tempMsg);

    // Output the sent data for verification
    Serial.print("Sent H: ");
    Serial.print(h);
    Serial.print("%, T: ");
    Serial.print(t);
    Serial.println(" C");

    Serial.print("TDS Value: ");
    Serial.print(tdsValue);
    Serial.println(" ppm");
    Serial.print("Sent Water Temp: ");
    Serial.print(waterTemp);
    Serial.println(" C");


    delay(5000); // Send one message per second
}
