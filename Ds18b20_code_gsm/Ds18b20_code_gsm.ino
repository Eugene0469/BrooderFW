#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h> //including software serial library
#include <DS3231.h>

#define DHTPIN 9     // Digital pin connected to the DHT sensor
#define DHTPIN_1 8     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


SoftwareSerial sim800l(A2, A3); // create a constructor of SoftwareSerial
char incomingByte; 
String inputString;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

#define ONE_WIRE_BUS A0 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
#define NUM_OF_SENSORS 8 //Number of connected sensors
int sen_number = 0; // Counter of Dallas sensors

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress T[NUM_OF_SENSORS]; // arrays to hold device addresses
float temp[NUM_OF_SENSORS];

DHT dht(DHTPIN, DHTTYPE);
DHT dht_1(DHTPIN_1, DHTTYPE);
static float h,h_1,dht_temp[2];

DS3231  rtc(SDA, SCL);
Time  t;

int count=0;
int relayPin = 7;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(20, 4);
  lcd.backlight();//To Power ON the back light
  dht.begin();
  dht_1.begin();
  rtc.begin();

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

  lcd.setCursor(0,0);
  lcd.print("  AgriQ & Aut Ltd    ");
  
  // SIM800L GSM Module Initialization
  sim800l.begin(9600);   // Setting the baud rate of GSM Module  
  if(!sim800l.available()){
   sim800l.println("AT");
   delay(1000); 
   Serial.println("Connecting...");
  }
  Serial.println("Connected!"); 
  sim800l.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(1000);
  sim800l.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS
  delay(1000);
  sim800l.println("AT+CMGL=\"REC UNREAD\""); // Read Unread Messages

  pinMode(relayPin, OUTPUT);
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
  readTemp();
  readHumidityValue_1();
  extractorFanCtrl();
//  readTextMessage();
  sendAlert();
}

void readTemp(){
  // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
  sensors.requestTemperatures(); 
  // print the device information
  //Remember to update the number of sensors.
  for(int i = 0; i<sensors.getDeviceCount(); i++)
  {
    temp[i] = sensors.getTempC(T[i]);
    if(temp[i] != -127.00)
    {
      lcd.setCursor(0,2);
      lcd.print("Zone ");
      lcd.print(i+2);
      lcd.print(": ");
      lcd.print(temp[i]); lcd.write((char)223); lcd.print("C "); 
//      i++;
//      if(i == sensors.getDeviceCount())break;
//      temp[i] = sensors.getTempC(T[i]);
//      if(temp[i] != -127.00)
//      { 
//        lcd.setCursor(0,2);
//        lcd.print("Zone ");
//        lcd.print(i+1);
//        lcd.print(" : ");
//        lcd.print(temp[i]); lcd.write((char)223); lcd.print("C ");  
//      }
      delay(2000);
    } 
  }
}

void readHumidityValue(){
    // Wait a few seconds between measurements.
  delay(2000);
//  lcd.setCursor(0,3);
//  lcd.print("                ");
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  dht_temp[0] = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h)) {
    Serial.println(F("Failed to read from DHT sensor!"));
//    lcd.setCursor(0,3);
//    lcd.print("Reading Humidity");
    return;
  }
  if(!(isnan(dht_temp[0])))
  {
    lcd.setCursor(0,1);
    lcd.print("Zone 1: ");
    lcd.print(dht_temp[0]);
    lcd.write((char)223); lcd.print("C ");
  }
  if(!(isnan(h)))
  {
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.println(F("%"));
    lcd.setCursor(0,3);
    lcd.print("Humidity: ");
    lcd.print(h);
    lcd.print("%");
  }
}

void readHumidityValue_1(){
    // Wait a few seconds between measurements.
//  delay(2000);
//  lcd.setCursor(0,3);
//  lcd.print("                ");
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h_1 = dht_1.readHumidity();
  dht_temp[1] = dht_1.readTemperature();
// Check if any reads failed and exit early (to try again).
  if (isnan(h_1)) {
    Serial.println(F("Failed to read from DHT_1 sensor!"));
//    lcd.setCursor(0,3);
//    lcd.print("Reading Humidity");
    return;
  }
  if(!(isnan(dht_temp[1])))
  {
    lcd.setCursor(0,1);
    lcd.print("Zone 4: ");
    lcd.print(dht_temp[1]);
    lcd.write((char)223); lcd.print("C ");
  }
  if(!(isnan(h_1)))
  {
    Serial.print(F("Humidity_1: "));
    Serial.print(h_1);
    Serial.println(F("%"));
    lcd.setCursor(0,3);
    lcd.print("Humidity 2 : ");
    lcd.print(h_1);
    lcd.print("%");
  }

}


void extractorFanCtrl()
{
  int ave_hum = 0;
  ave_hum = (h+h_1)/2;
  if(ave_hum >= 80)
  {
    digitalWrite(relayPin, LOW); //The relay module is active low
  }
  else if (ave_hum <= 60)
  {
    digitalWrite(relayPin, HIGH);
  }
}
void SendTextMessage()
{
  Serial.println("Sending Text...");
  sim800l.print("AT+CMGF=1\n"); // Set the shield to SMS mode
  delay(100);
  sim800l.print("AT+CMGS=\"+254710781496\"\n");  
  delay(5000);
//  sim800l.print("This is a Test text message from SIM800L "); //the content of the message
  sim800l.print("Zone 1 : "); //the content of the message
  sim800l.print(dht_temp[0]); //the content of the message
  sim800l.print(" Celsius\n");
  for(int i = 0; i<sensors.getDeviceCount(); i++)
  {
    if(temp[i] != -127.00)
    {
      sim800l.print("Zone "); //the content of the message
      sim800l.print(i+2);
      sim800l.print(" : ");
      sim800l.print(temp[i]); 
//      sim800l.print((char)177); 
      sim800l.print(" Celsius\n");
    } 
  }
  sim800l.print("Zone 4 : "); //the content of the message
  sim800l.print(dht_temp[1]); //the content of the message
  sim800l.print(" Celsius\n");
  sim800l.print("Humidity 1 : "); //the content of the message
  sim800l.print(h); //the content of the message
  sim800l.println("%");
  sim800l.print("Humidity 2 : "); //the content of the message
  sim800l.print(h_1); //the content of the message     
  sim800l.println("%");
  sim800l.print("\n"); 
  delay(500);
  sim800l.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(10000);

  sim800l.print("AT+CMGS=\"+254736184663\"\n");  
  delay(5000);
//  sim800l.print("This is a Test text message from SIM800L "); //the content of the message
  sim800l.print("Zone 1 : "); //the content of the message
  sim800l.print(dht_temp[0]); //the content of the message
  sim800l.print(" Celsius\n");
  for(int i = 0; i<sensors.getDeviceCount(); i++)
  {
    if(temp[i] != -127.00)
    {
      sim800l.print("Zone "); //the content of the message
      sim800l.print(i+2);
      sim800l.print(" : ");
      sim800l.print(temp[i]); 
//      sim800l.print((char)177); 
      sim800l.print(" Celsius\n");
    } 
  }
  sim800l.print("Zone 4 : "); //the content of the message
  sim800l.print(dht_temp[1]); //the content of the message
  sim800l.print(" Celsius\n");
  sim800l.print("Humidity 1 : "); //the content of the message
  sim800l.print(h); //the content of the message
  sim800l.println("%");
  sim800l.print("Humidity 2 : "); //the content of the message
  sim800l.print(h_1); //the content of the message     
  sim800l.println("%");
  sim800l.print("\n"); 
  delay(500);
  sim800l.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
//  sim800l.println();
  Serial.println("Text Sent.");
  delay(500);
}


//void readTextMessage(){
//   delay(100);   // Serial Buffer
//   while(sim800l.available()){
//   incomingByte = sim800l.read();
//   inputString += incomingByte; 
//  }
//  delay(10);      
//
//  Serial.println(inputString);
//  inputString.toUpperCase(); // Uppercase the Received Message
//
//  //turn RELAY ON or OFF
//  if (inputString.indexOf("READ") > -1)
//  {
//    SendTextMessage();
//  }
//  if (inputString.indexOf("ON") > -1)
//  {
//    SendClientTextMessage();
//  }
//  delay(2000);
//  //Delete Messages & Save Memory
//  if (inputString.indexOf("OK") == -1){
//  sim800l.println("AT+CMGDA=\"DEL ALL\"");
//
//  delay(1000);}
//
//  inputString = "";  
//}

void sendAlert(){
  // Get data from the DS3231
  t = rtc.getTime();
  int rem = t.min%30;
//  Serial.println(rtc.getDateStr());
//  lcd.setCursor(0,0);
//  lcd.print(rtc.getTimeStr());
  if(rem==0 && count ==0){
    //activate relay
    SendTextMessage();
    count++;
  }
  else if(rem>0)
  {
    count=0;
  }
}
