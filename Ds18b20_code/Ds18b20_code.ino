#include <LiquidCrystal.h>
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS A0 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
#define NUM_OF_SENSORS 9 //Number of connected sensors
int sen_number = 0; // Counter of Dallas sensors

OneWire oneWire(ONE_WIRE_BUS);
OneWire  ds(2);  // on pin 10 (a 4.7K resistor is necessary)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress T[NUM_OF_SENSORS]; // arrays to hold device addresses
float temp[NUM_OF_SENSORS];

void setup(void)
{
  lcd.begin(20,4);
  Serial.begin(9600); //Start serial port
  Serial.println("Dallas Temperature IC Control Library");
  // Start up the library
  sensors.begin();
  // locate devices on the bus
  Serial.print("Found: ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" Devices.");
  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  // Search for devices on the bus and assign based on an index.
  
  for(int i=0; i<sensors.getDeviceCount();i++)
  {
    if (!sensors.getAddress(T[i], i)) 
    {
      Serial.print("Not Found Sensor ");
      Serial.println(i+1);
    }
  } 
    
  // show the addresses we found on the bus
  for (int k =0; k < sensors.getDeviceCount(); k++) {
    Serial.print("Sensor "); Serial.print(k+1);
    Serial.print(" Address: ");
    printAddress(T[k]); Serial.println();
  }
  // set the resolution to 12 bit per device
  for(int i=0;i<sensors.getDeviceCount();i++)
  {
    sensors.setResolution(T[i], precision);
  }
  for (int k =0; k < sensors.getDeviceCount(); k++) {
    Serial.print("Sensor "); Serial.print(k+1);
    Serial.print(" Resolution: ");
    Serial.print(sensors.getResolution(T[k]), DEC); Serial.println();
  }
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp : ");
  Serial.print(tempC);
  Serial.print(" Celcius degres ");
}


void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
  Serial.print("Reading DATA..."); sensors.requestTemperatures(); Serial.println("DONE");
  // print the device information
  for (int k =0; k < sensors.getDeviceCount(); k++) {
    Serial.print("Sensor "); Serial.print(k+1); Serial.print(" ");
    printData(T[k]);
  }

//  for(int i = 0; i<sensors.getDeviceCount(); i++)
//  {
//    temp[i] = sensors.getTempC(T[i]);
//    if(temp[i] != -127.00)
//    {
//      lcd.setCursor(0,0);
//      lcd.print("Sensor Number ");
//      lcd.print(i+1);
//      lcd.setCursor(0,1);
//      lcd.print(" Temp: ");
//      lcd.print(temp[i]); lcd.write((char)223); lcd.print("C "); 
//      delay(1200);     
//    }  
//    delay(1000);
//  }
  }
