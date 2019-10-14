#include "DHT.h"
#include <LiquidCrystal.h>
#include <DS3231.h>
#include <SD.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DHTPIN 9     // Digital pin connected to the DHT sensor
#define DHTPIN_1 8     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);
DHT dht_1(DHTPIN_1, DHTTYPE);
static float h,h_1;

DS3231  rtc(SDA, SCL);
Time  t; 

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

File myFile;
int pinCS = 10  ;
int count = 0;

#define ONE_WIRE_BUS A0 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
int sen_number = 0; // Counter of Dallas sensors

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress T[6]; // arrays to hold device addresses
float temp[6];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  dht_1.begin();
  lcd.begin(16, 2);
  rtc.begin();  
  pinMode(pinCS, OUTPUT);

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
  
  if (!sensors.getAddress(T[0], 0)) Serial.println("Not Found Sensor 1");
  if (!sensors.getAddress(T[1], 1)) Serial.println("Not Found Sensor 2");
  if (!sensors.getAddress(T[2], 2)) Serial.println("Not Found Sensor 3");
  if (!sensors.getAddress(T[3], 3)) Serial.println("Not Found Sensor 4");
  if (!sensors.getAddress(T[4], 4)) Serial.println("Not Found Sensor 5");
  if (!sensors.getAddress(T[5], 5)) Serial.println("Not Found Sensor 6");
  
  // show the addresses we found on the bus
  for (int k =0; k < sensors.getDeviceCount(); k++) {
    Serial.print("Sensor "); Serial.print(k+1);
    Serial.print(" Address: ");
    if (k == 0) { printAddress(T[0]); Serial.println();
      } else if (k == 1) { printAddress(T[1]); Serial.println();
       } else if (k == 2) { printAddress(T[2]); Serial.println();
        } else if (k == 3) { printAddress(T[3]); Serial.println();
         } else if (k == 4) { printAddress(T[4]); Serial.println();
          } else if (k == 5) { printAddress(T[5]); Serial.println();
           }  
  }
  // set the resolution to 12 bit per device
  sensors.setResolution(T[0], precision);
  sensors.setResolution(T[1], precision);
  sensors.setResolution(T[2], precision);
  sensors.setResolution(T[3], precision);
  sensors.setResolution(T[4], precision);
  sensors.setResolution(T[5], precision);
  
  for (int k =0; k < sensors.getDeviceCount(); k++) {
  Serial.print("Sensor "); Serial.print(k+1);
  Serial.print(" Resolution: ");
  if (k == 0) { Serial.print(sensors.getResolution(T[0]), DEC); Serial.println();
    } else if (k == 1) { Serial.print(sensors.getResolution(T[1]), DEC); Serial.println();
     } else if (k == 2) { Serial.print(sensors.getResolution(T[2]), DEC); Serial.println();
      } else if (k == 3) { Serial.print(sensors.getResolution(T[3]), DEC); Serial.println();
       } else if (k == 4) { Serial.print(sensors.getResolution(T[4]), DEC); Serial.println();
        } else if (k == 5) { Serial.print(sensors.getResolution(T[5]), DEC); Serial.println();
         }
  }

  lcd.setCursor(0,0);
  lcd.print("Brooder Bora");
  
  // SD Card Initialization
  if (SD.begin())Serial.println("SD card is ready to use."); 
  else{Serial.println("SD card initialization failed");return;}
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

void loop() {
  // put your main code here, to run repeatedly:
  readHumidityValue();
  readHumidityValue_1();
  readTemp();
  logData();
}

void readHumidityValue(){
    // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h)) {
//    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println(F("%"));
}

void readHumidityValue_1(){
    // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h_1 = dht_1.readHumidity();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h_1)) {
//    Serial.println(F("Failed to read from DHT_1 sensor!"));
    return;
  }
  Serial.print(F("Humidity_1: "));
  Serial.print(h_1);
  Serial.println(F("%"));
}

void readTemp(){
  // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
//  Serial.print("Reading DATA..."); 
    sensors.requestTemperatures(); 
//    Serial.println("DONE");
  // print the device information
  for (int k =0; k < sensors.getDeviceCount(); k++) {
//    Serial.print("Sensor "); Serial.print(k+1); Serial.print(" ");
//    printData(T[k]);
 
  }
  for(int i = 0; i<sensors.getDeviceCount(); i++)
  {
    temp[i] = sensors.getTempC(T[i]);
    if(temp[i] != -127.00)
    {
      lcd.setCursor(0,1);
      lcd.print("Zone ");
      lcd.print(i+1);
       lcd.print(" : ");
      lcd.print(temp[i]); lcd.write((char)223); lcd.print("C ");  
      delay(2000);
    
    }  
  }
}

void logData(){
  t = rtc.getTime();
  int rem = t.min%10;
  if(rem==0 && count==0)
  {
    Serial.print(rtc.getDateStr());
    Serial.print(" ");
    Serial.print(rtc.getTimeStr());
    for(int i=0; i<sensors.getDeviceCount();i++)
    {
      Serial.print(",");    
      Serial.print(temp[i]);        
    }
    Serial.print(",");
    Serial.print(h);
    Serial.print(",");
    Serial.println(h_1);
    myFile = SD.open("log.txt", FILE_WRITE);
    if (myFile) { 
      myFile.print(rtc.getDateStr()); 
      myFile.print(",");
      myFile.print(rtc.getTimeStr());
      for(int i=0; i<sensors.getDeviceCount();i++)
      {
        myFile.print(",");    
        myFile.print(temp[i]);        
      }
      myFile.print(",");    
      myFile.print(h);
      myFile.print(",");    
      myFile.println(h_1);
      myFile.close(); // close the file
    }
    // if the file didn't open, print an error:
    else {
      Serial.println("error opening test.txt");
    }   
    count++;
    delay(1000);
  }
  else if(rem>0)
  {
    count=0;
  }
}
