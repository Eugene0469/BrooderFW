#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

//Select between reading of the address and reading of the temperature.

#define ENABLE_DREAD 0 //enables temperature data reading.
#define ENABLE_AREAD 0 //enables device address reading 


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
#if ENABLE_AREAD
// variable to hold device addresses
DeviceAddress Thermometer;

int deviceCount = 0;
#endif

#if ENABLE_DREAD
// Addresses of 3 DS18B20s.Replace with own addresses
uint8_t sensor1[8] = { 0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x02, 0xEC };
uint8_t sensor2[8] = { 0x28, 0x61, 0x64, 0x12, 0x3C, 0x7C, 0x2F, 0x27 };
uint8_t sensor3[8] = { 0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6 };
#endif

void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
  #if ENABLE_AREAD
     // locate devices on the bus
    Serial.println("Locating devices...");
    Serial.print("Found ");
    deviceCount = sensors.getDeviceCount();
    Serial.print(deviceCount, DEC);
    Serial.println(" devices.");
    Serial.println("");
    
    Serial.println("Printing addresses...");
    for (int i = 0;  i < deviceCount;  i++)
    {
      Serial.print("Sensor ");
      Serial.print(i+1);
      Serial.print(" : ");
      sensors.getAddress(Thermometer, i);
      printAddress(Thermometer);
    }
   #endif
}

void loop(void)
{
  #if ENABLE_DREAD
    sensors.requestTemperatures();
    
    Serial.print("Sensor 1: ");
    printTemperature(sensor1);
    
    Serial.print("Sensor 2: ");
    printTemperature(sensor2);
    
    Serial.print("Sensor 3: ");
    printTemperature(sensor3);
    
    Serial.println();
    delay(1000);
  #endif
}

#if ENABLE_DREAD
  void printTemperature(DeviceAddress deviceAddress)
  {
    float tempC = sensors.getTempC(deviceAddress);
    Serial.print(tempC);
    Serial.print((char)176);
    Serial.print("C  |  ");
    Serial.print(DallasTemperature::toFahrenheit(tempC));
    Serial.print((char)176);
    Serial.println("F");
  }
#endif

#if ENABLE_AREAD
  void printAddress(DeviceAddress deviceAddress)
  { 
    for (uint8_t i = 0; i < 8; i++)
    {
      Serial.print("0x");
      if (deviceAddress[i] < 0x10) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
      if (i < 7) Serial.print(", ");
    }
    Serial.println("");
  }
#endif
