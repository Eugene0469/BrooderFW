//#include "DHT.h"
//#include <Wire.h> 
//#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h> //including software serial library
#include <DS3231.h>
// #include <Adafruit_FONA.h>
#include <LiquidCrystal.h>

#define DHTPIN 9     // Digital pin connected to the DHT sensor
#define DHTPIN_1 8     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define FONA_RX A3
#define FONA_TX A2
// #define FONA_RST 7

SoftwareSerial sim800l(FONA_TX, FONA_RX); // create a constructor of SoftwareSerial
// Adafruit_FONA SIM800 = Adafruit_FONA(FONA_RST);

// char http_cmd[80];
// char url_string[] = "api.thingspeak.com/update.json?api_key=MVEE68WLO22F1PQ3&field1";
// double atm_pressure = 0;
// char atm_pressure_string[200];
// int8_t net_status;

// boolean gprs_on = false;
// boolean tcp_on = false;

//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define ONE_WIRE_BUS A0 // Data wire is plugged into port 9 on the Arduino
#define precision 12 // OneWire precision Dallas Sensor
#define NUM_OF_SENSORS 9 //Number of connected sensors
int8_t sen_number = 0; // Counter of Dallas sensors

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress T[NUM_OF_SENSORS]; // arrays to hold device addresses
float temp[NUM_OF_SENSORS];

//DHT dht(DHTPIN, DHTTYPE);
//DHT dht_1(DHTPIN_1, DHTTYPE);
//static float hum[3];

DS3231  rtc(SDA, SCL);

int count=0, countTwo=0;

//#define relayPin  8

void SystemInit()      //Function to Send Message
{
  Serial.println("System Restart");
  
  sim800l.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode

  delay(1000);  

  sim800l.println("AT+CMGS=\"+254721460975\"\r"); // Replace x with mobile number

  delay(1000);

  sim800l.println("System Restart"); // The SMS text you want to send

  delay(100);

  sim800l.println((char)26);// ASCII code of CTRL+Z

  delay(1000);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(20, 4);
//  lcd.backlight();//To Power ON the back light
//  dht.begin();
//  dht_1.begin();
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

//  lcd.setCursor(0,0);
//  lcd.print("A&A LTD");
  
  // SIM800L GSM Module Initialization
  sim800l.begin(9600);   // Setting the baud rate of GSM Module  
  while(!sim800l.available()){
   sim800l.println("AT");
   delay(1000); 
   Serial.println("Connecting...");
   lcd.setCursor(0,0);
   lcd.print("Connecting...");
  }
  Serial.println("Connected!"); 
  sim800l.println("AT+CMGDA=\"DEL ALL\"");
  delay(2000);
  sim800l.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(1000);
  sim800l.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS
  delay(1000);
   sim800l.println("AT+CMGL=\"REC UNREAD\""); // Read Unread Messages
  SystemInit();
  delay(2000);
  
//  pinMode(relayPin, OUTPUT);
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
//  sim800l.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(2000);
//  lcd.clear();
  // sendValuesToThingSpeak();
  //  extractorFanCtrl();
  readTemp();
//  readHumidityValue();
//  readHumidityValue_1();
  if(sim800l.available())
  {
    delay(1000);
    readTextMessage();
  }
  sendAlert();
}

void readTemp(){
  // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
  sensors.requestTemperatures(); 
  // print the device information
  //Remember to update the number of sensors.
  for (int k =0; k < sensors.getDeviceCount(); k++) {
    Serial.print("Sensor "); Serial.print(k+1); Serial.print(" ");
    printData(T[k]);
  }
  for(int i = 0; i<sensors.getDeviceCount(); i++)
  {
    temp[i] = sensors.getTempC(T[i]);
    if(temp[i] != -127.00)
    {
      lcd.setCursor(0,i);
      lcd.print("Zone ");
      lcd.print(i+1);
      lcd.print(": ");
      lcd.print(temp[i]); lcd.write((char)223); lcd.print("C "); 
//      i++;
//      if(i == sensors.getDeviceCount())break;
//      temp[i] = sensors.getTempC(T[i]);
//      if(temp[i] != -127.00)
//      { 
//        lcd.setCursor(0,1);
//        lcd.print("Zone ");
//        lcd.print(i+1);
//        lcd.print(": ");
//        lcd.print(temp[i]); lcd.write((char)223); lcd.print("C ");  
//      }
//      delay(2000);
    } 
  }
}

//void readHumidityValue(){
//    // Wait a few seconds between measurements.
//  delay(2000);
//  //  lcd.setCursor(0,3);
//  //  lcd.print("                ");
//  // Reading temperature or humidity takes about 250 milliseconds!
//  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
//  hum[0] = dht.readHumidity();
//  // Check if any reads failed and exit early (to try again).
//  if (isnan(hum[0])) {
//    Serial.println(F("Failed to read from DHT sensor!"));
//    return;
//  }
//
//  if(!(isnan(hum[0])))
//  {
//    Serial.print(F("Humidity: "));
//    Serial.print(hum[0]);
//    Serial.println(F("%"));
//    lcd.setCursor(0,2);
//    lcd.print("Hum 1 : ");
//    lcd.print(hum[0]);
//    lcd.print("% ");
//  }
//}
//
//void readHumidityValue_1(){
//    // Wait a few seconds between measurements.
//  delay(2000);
//  //  lcd.setCursor(0,3);
//  //  lcd.print("                ");
//    // Reading temperature or humidity takes about 250 milliseconds!
//    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
//    hum[1] = dht_1.readHumidity();
//  // Check if any reads failed and exit early (to try again).
//  if (isnan(hum[1])) {
//    Serial.println(F("Failed to read from DHT_1 sensor!"));
//    return;
//  }
//  if(!(isnan(hum[1])))
//  {
//    Serial.print(F("Humidity_1: "));
//    Serial.print(hum[1]);
//    Serial.println(F("%"));
//    lcd.setCursor(0,3);
//    lcd.print("Hum 2 : ");
//    lcd.print(hum[1]);
//    lcd.print("% ");
//  }
//
//}

//void extractorFanCtrl()
//{
//  int ave_hum = 0;
//  ave_hum = (hum[0]+hum[1])/2;
//  if(ave_hum >= 80)
//  {
//    digitalWrite(relayPin, LOW); //The relay module is active low
//  }
//  else if (ave_hum <= 60)
//  {
//    digitalWrite(relayPin, HIGH);
//  }
//}

void ClientRequest()
{
  Serial.println("Sending Text...");
  sim800l.print("AT+CMGF=1\n"); // Set the shield to SMS mode
  delay(1000);
  sim800l.print("AT+CMGS=\"+254726360279\"\n");  
  delay(1000);
  SendTextMessage();
  delay(10000);

//  sim800l.print("AT+CMGF=1\n"); // Set the shield to SMS mode
//  delay(1000);
//  sim800l.print("AT+CMGS=\"+254736184663\"\n");  
//  delay(1000);
//  SendTextMessage();
  
}

void ClientTwoRequest()
{
  Serial.println("Sending Text...");
  sim800l.print("AT+CMGF=1\n"); // Set the shield to SMS mode
  delay(1000);
  sim800l.print("AT+CMGS=\"+254722729766\"\n");  
  delay(1000);
  SendTextMessage();
  delay(10000);
}

void SystemCheck()
{
    Serial.println("Sending Text...");
    sim800l.print("AT+CMGF=1\n"); // Set the shield to SMS mode
    delay(1000);
    sim800l.print("AT+CMGS=\"+254728480518\"\n");  
    delay(1000);
    SendTextMessage();
  //  sim800l.print("This is a Test text message from SIM800L "); //the content of the message
    
}
void SendTextMessage()
{
  for(int i = 0; i<sensors.getDeviceCount(); i++)
  {
    if(temp[i] != -127.00)
    {
      sim800l.print("Zone "); //the content of the message
      sim800l.print(i+1);
      sim800l.print(": ");
      sim800l.print(temp[i]); 
//      sim800l.print((char)177); 
      sim800l.print(" Celsius\n");
    } 
  }
//  if (!(isnan(hum[0]))) {
//    sim800l.print("Humidity 1: "); //the content of the message
//    sim800l.print(hum[0]); //the content of the message
//    sim800l.println("%");
//  }
//  if (!(isnan(hum[1]))) {
//   sim800l.print("Humidity 2: "); //the content of the message
//   sim800l.print(hum[1]); //the content of the message     
//   sim800l.println("%");
//  }
  sim800l.print("\n"); 
  delay(1000);
  sim800l.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(1000);
}
void readTextMessage()
{
    // char incomingByte; 
    String inputString;
    // delay(1000);   // Serial Buffer
    while(sim800l.available()){
    inputString = sim800l.readString();
    // inputString += incomingByte; 
   }
   delay(500);      
  
   Serial.println(inputString);
   inputString.toUpperCase(); // Uppercase the Received Message
  
   //Send text message
   if (inputString.indexOf("CHECK") > -1)
   {
      ClientRequest();
      // inputString ="";  
   }

   if (inputString.indexOf("READ") > -1)
   {
      SystemCheck();
      // inputString ="";  
   }

   if (inputString.indexOf("REQUEST") > -1)
   {
      ClientTwoRequest();
      // inputString ="";  
   }

   delay(2000);
   //Delete Messages & Save Memory
   if (inputString.indexOf("OK") == -1)
  {
   sim800l.println("AT+CMGDA=\"DEL ALL\"");
   delay(2000);
  }
  inputString =""; 
}

void sendAlert(){
  // Get data from the DS3231
  Time  t;
  t = rtc.getTime();
  int rem = t.min%30;
  Serial.println(rtc.getTimeStr());
//  lcd.setCursor(12,0);
//  lcd.print(rtc.getTimeStr());
  if(rem==0 && count ==0){
    //activate relay
    ClientRequest();
    count++;
  }
  else if(rem>0)
  {
    count=0;
  }
//B 
  
}
