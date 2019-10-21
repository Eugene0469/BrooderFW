#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h> //including software serial library

SoftwareSerial sim800l(A2, A3); // create a constructor of SoftwareSerial
char incomingByte; 
String inputString;

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define ONE_WIRE_BUS A0 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
#define NUM_OF_SENSORS 7 //Number of connected sensors
int sen_number = 0; // Counter of Dallas sensors

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress T[NUM_OF_SENSORS]; // arrays to hold device addresses
float temp[NUM_OF_SENSORS];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(20, 4);

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
  lcd.print("Brooder Bora");
  
  // SIM800L GSM Module Initialization
  sim800l.begin(9600);   // Setting the baud rate of GSM Module  
  while(!sim800l.available()){
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
  readTemp();
  readTextMessage();
}

void readTemp(){
  // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
    sensors.requestTemperatures(); 
  // print the device information
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
//      delay(2000);
    }  
  }
}

void SendTextMessage()
{
  Serial.println("Sending Text...");
  sim800l.print("AT+CMGF=1\n"); // Set the shield to SMS mode
  delay(100);
  sim800l.print("AT+CMGS=\"+254721460975\"\n");  
  delay(200);
//  sim800l.print("This is a Test text message from SIM800L "); //the content of the message
  for(int i = 0; i<sensors.getDeviceCount(); i++)
  {
    if(temp[i] != -127.00)
    {
      sim800l.print("Zone "); //the content of the message
      sim800l.print(i+1);
      sim800l.print(" : ");
      sim800l.print(temp[i]); 
//      sim800l.print((char)223); 
      sim800l.print(" C\n ");
    } 
  }   
  sim800l.print("\n"); 
  delay(500);
  sim800l.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
//  sim800l.println();
  Serial.println("Text Sent.");
  delay(500);
}
void readTextMessage(){
   delay(100);   // Serial Buffer
   while(sim800l.available()){
   incomingByte = sim800l.read();
   inputString += incomingByte; 
  }
  delay(10);      

  Serial.println(inputString);
  inputString.toUpperCase(); // Uppercase the Received Message

  //turn RELAY ON or OFF
  if (inputString.indexOf("ON") > -1){
    SendTextMessage();
    }
  if (inputString.indexOf("+254") > -1){
      ChangeNumber();
    }    
    
  delay(2000);
//  //Delete Messages & Save Memory
//  if (inputString.indexOf("OK") == -1){
//  sim800l.println("AT+CMGDA=\"DEL ALL\"");
//
//  delay(1000);}

  inputString = "";   

}