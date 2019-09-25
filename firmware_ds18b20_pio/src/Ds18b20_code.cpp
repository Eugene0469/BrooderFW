#include <Arduino.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(11, 12, 6, 7, 8, 9);
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
int sen_number = 0; // Counter of Dallas sensors

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress T1, T2, T3, T4; // arrays to hold device addresses

void printAddress(DeviceAddress deviceAddress);
void printTemperature(DeviceAddress deviceAddress);
void printData(DeviceAddress deviceAddress);


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
  
  if (!sensors.getAddress(T1, 0)) Serial.println("Not Found Sensor 1");
  if (!sensors.getAddress(T2, 1)) Serial.println("Not Found Sensor 2");
  if (!sensors.getAddress(T3, 2)) Serial.println("Not Found Sensor 3");
  if (!sensors.getAddress(T4, 3)) Serial.println("Not Found Sensor 4");
  
  
  // show the addresses we found on the bus
  for (int k =0; k < sensors.getDeviceCount(); k++) {
    Serial.print("Sensor "); Serial.print(k+1);
    Serial.print(" Address: ");
    if (k == 0) { printAddress(T1); Serial.println();
      } else if (k == 1) { printAddress(T2); Serial.println();
       } else if (k == 2) { printAddress(T3); Serial.println();
        } else if (k == 3) { printAddress(T4); Serial.println();
         }  
  }
  // set the resolution to 12 bit per device
  sensors.setResolution(T1, precision);
  sensors.setResolution(T2, precision);
  sensors.setResolution(T3, precision);
  sensors.setResolution(T4, precision);
  
  for (int k =0; k < sensors.getDeviceCount(); k++) {
  Serial.print("Sensor "); Serial.print(k+1);
  Serial.print(" Resolution: ");
  if (k == 0) { Serial.print(sensors.getResolution(T1), DEC); Serial.println();
    } else if (k == 1) { Serial.print(sensors.getResolution(T2), DEC); Serial.println();
     } else if (k == 2) { Serial.print(sensors.getResolution(T3), DEC); Serial.println();
      } else if (k == 3) { Serial.print(sensors.getResolution(T4), DEC); Serial.println();
       }  
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
    if (k == 0) { printData(T1);
      } else if (k == 1) { printData(T2);
       } else if (k == 2) { printData(T3);
        } else if (k == 3) { printData(T4);
         }  
  }
  if (sen_number == sensors.getDeviceCount()) {
    sen_number = 0; // reset counter
    // lcd.clear(); // clear screen on LCD
  }
  lcd.setCursor(0,0);
  lcd.print("Sensor 1: ");
  lcd.print(sensors.getTempC(T1)); lcd.write((char)223); lcd.print("C   ");
  lcd.setCursor(0,1);
  lcd.print("Sensor 2: ");
  lcd.print(sensors.getTempC(T2)); lcd.write((char)223); lcd.print("C   ");
  lcd.setCursor(0,2);
  lcd.print("Sensor 3: ");
  lcd.print(sensors.getTempC(T3)); lcd.write((char)223); lcd.print("C   ");
  lcd.setCursor(0,3);
  lcd.print("Sensor 4: ");
  lcd.print(sensors.getTempC(T4)); lcd.write((char)223); lcd.print("C   ");
  
  Serial.print("Sensor Number="); Serial.println(sen_number);
  delay(2000);
  sen_number++ ;
}
