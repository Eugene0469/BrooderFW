//#include <LiquidCrystal.h>
//const int rs = 23, en = 25, d4 = 27, d5 = 29, d6 = 31, d7 = 33;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//#include <Wire.h> 

#define ONE_WIRE_BUS PB13 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
#define NUM_OF_SENSORS 9 //Number of connected sensors
int sen_number = 0; // Counter of Dallas sensors

OneWire oneWire(ONE_WIRE_BUS);
//OneWire  ds(2);  // on pin 10 (a 4.7K resistor is necessary)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress T[NUM_OF_SENSORS]; // arrays to hold device addresses
float temp[NUM_OF_SENSORS];

//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

void setup(void)
{
//  lcd.begin(20,4);
//  lcd.begin(20, 4);
//  lcd.backlight();//To Power ON the back light
  Serial.begin(9600); //Start serial port
  Serial.println("Dallas Temperature IC Control Library");
  // Start up the library
  sensors.begin();
  // locate devices on the bus
  Serial.print("Found: ");
  Serial.print(sensors.getDeviceCount(), DEC);
  delay(1000);
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
  Serial.print("Reading DATA..."); 
  sensors.requestTemperatures(); 
  Serial.println("DONE");
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
//      lcd.setCursor(0,i);
//      lcd.print("Zone ");
//      lcd.print(i+1);
////      lcd.setCursor(0,1);
//      lcd.print(": ");
//      lcd.print(temp[i]); lcd.write((char)223); lcd.print("C "); 
////      delay(1200);     
//    }  
//    delay(1000);
//  }
  }
// DS3231_Serial_Easy
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my DS3231-library to 
// quickly send time and date information over a serial link
//
// To use the hardware I2C (TWI) interface of the Arduino you must connect
// the pins as follows:
//
// Arduino Uno/2009:
// ----------------------
// DS3231:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
//          SCL pin   -> Arduino Analog 5 or the dedicated SCL pin
//
// Arduino Leonardo:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 2 or the dedicated SDA pin
//          SCL pin   -> Arduino Digital 3 or the dedicated SCL pin
//
// Arduino Mega:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA pin
//          SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL pin
//
// Arduino Due:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA1 (Digital 70) pin
//          SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL1 (Digital 71) pin
//
// The internal pull-up resistors will be activated when using the 
// hardware I2C interfaces.
//
// You can connect the DS3231 to any available pin but if you use any
// other than what is described above the library will fall back to
// a software-based, TWI-like protocol which will require exclusive access 
// to the pins used, and you will also have to use appropriate, external
// pull-up resistors on the data and clock signals.


//#include <DS3231.h>
//
//// Init the DS3231 using the hardware interface
//DS3231  rtc(SDA, SCL);
//
//void setup()
//{
//  // Setup Serial connection
//  Serial.begin(9600);
//  // Uncomment the next line if you are using an Arduino Leonardo
//  //while (!Serial) {}
//  Serial.println("Hello");
//  // Initialize the rtc object
//  rtc.begin();
//  
//  // The following lines can be uncommented to set the date and time
//  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
//  //rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
//  //rtc.setDate(1, 1, 2014);   // Set the date to January 1st, 2014
//}
//
//void loop()
//{
//  // Send Day-of-Week
//  Serial.print(rtc.getDOWStr());
//  Serial.print(" ");
//  
//  // Send date
//  Serial.print(rtc.getDateStr());
//  Serial.print(" -- ");
//
//  // Send time
//  Serial.println(rtc.getTimeStr());
//  
//  // Wait one second before repeating :)
//  delay (1000);
//}
