#include <DHT.h>                    //Library for using DHT sensor 
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Wire.h"
#include <LiquidCrystal_I2C.h>

#define DS18B20     0
#define DHT22       0
#define LCD_I2C     0
#define SIM800L     0

#if DS18B20
    #define ONE_WIRE_BUS PB12 // Data wire is plugged into port PB13 on the Blue Pill
    #define precision 12 // OneWire precision Dallas Sensor
    #define NUM_OF_SENSORS 9 //Number of connected sensors
    int sen_number = 0; // Counter of Dallas sensors
    OneWire oneWire(ONE_WIRE_BUS);
    DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
    DeviceAddress T[NUM_OF_SENSORS]; // arrays to hold device addresses
    float temp[NUM_OF_SENSORS];
#endif

#if DHT22
    #define DHTPIN PB13 //dht22 sensor is connected to pin PB13 of the bluepill
    #define DHTTYPE DHT22
    DHT dht(DHTPIN, DHTTYPE);     //initilize object dht for class DHT with DHT pin with STM32 and DHT type as DHT22
    float h; // Variable to hold the value of the r/h measured 
#endif

#if LCD_I2C
    LiquidCrystal_I2C lcd(0x27, 20, 4);
#endif

#define DS3231_I2C_ADDRESS 0x68
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(int val)
{
  return ( (val / 10 * 16) + (val % 10) );
}
// Convert binary coded decimal to normal decimal numbers
int bcdToDec(int val)
{
  return ( (val / 16 * 10) + (val % 16) );
}

int count = 0, countTwo = 0;

#if SIM800L
    void SystemInit()      //Function to Send Message
    {
      Serial.println("System Restart");
    
      Serial1.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
    
      delay(1000);
    
      Serial1.println("AT+CMGS=\"+254721460975\"\r"); // Replace x with mobile number
    
      delay(1000);
    
      Serial1.print("System Restart"); // The SMS text you want to send
    
      delay(100);
    
      Serial1.print((char)26);// ASCII code of CTRL+Z
    
      delay(1000);
    }
#endif

void setup()
{
  #if LCD_I2C
      lcd.begin();
      lcd.backlight();//To Power ON the back light
  #endif
  Wire.begin();
  Serial.begin(9600);
  #if DHT22
      dht.begin();          //Begins to receive Temperature and humidity values.
  #endif
  #if DS18B20
      Serial.println("Dallas Temperature IC Control Library");
      // Start up the library
      sensors.begin();
      // locate devices on the bus
      Serial.print("Found: ");
      Serial.print(sensors.getDeviceCount(), DEC);
      delay(1000);
      Serial.println(" Devices.");
      for (int i = 0; i < sensors.getDeviceCount(); i++)
      {
        if (!sensors.getAddress(T[i], i))
        {
          Serial.print("Not Found Sensor ");
          Serial.println(i + 1);
        }
      }
      // set the resolution to 12 bit per device
      for (int i = 0; i < sensors.getDeviceCount(); i++)
      {
        sensors.setResolution(T[i], precision);
      }
  #endif
  #if SIM800L
      Serial1.begin(9600);   // Setting the baud rate of GSM Module
      while (!Serial1.available()) {
        Serial1.println("AT");
        delay(1000);
        Serial.println("Connecting...");
        #if LCD_I2C
            lcd.setCursor(0, 0);
            lcd.print("Connecting...");
        #endif
      }
      Serial.println("Connected!");
      #if LCD_I2C
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Connected");
      #endif
      Serial1.println("AT+CMGDA=\"DEL ALL\"");
      delay(2000);
      Serial1.println("AT+CMGF=1"); // Configuring TEXT mode
      delay(1000);
      Serial1.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS
      delay(1000);
      Serial1.println("AT+CMGL=\"REC UNREAD\""); // Read Unread Messages
      delay(2000);
      SystemInit();
      delay(2000);
  #endif
  #if LCD_I2C
      lcd.clear();
  #endif
  //  while(Serial1.available())
  //  {
  //    Serial.write(Serial1.read());
  //  }
}

void loop()
{
  #if DS18B20
      sensors.requestTemperatures();
      for (int i = 0; i < sensors.getDeviceCount(); i++)
      {
        temp[i] = sensors.getTempC(T[i]);
        if (temp[i] != -127.00f)
        {
          Serial.print("Zone ");
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.print(temp[i]);
          //      Serial.write((char)248);
          Serial.println(" C");
          #if LCD_I2C
              lcd.setCursor(0, i);
              lcd.print("Zone ");
              lcd.print(i + 1);
              lcd.print(": ");
              lcd.print(temp[i]); lcd.write((char)223); lcd.print("C ");
          #endif
          //      i++;
          //      if(i == sensors.getDeviceCount())break;
          //      temp[i] = sensors.getTempC(T[i]);
          //      if(temp[i] != -127.00)
          //      {
          //        #if LCD_I2C
              //        lcd.setCursor(0,1);
              //        lcd.print("Zone ");
              //        lcd.print(i+1);
              //        lcd.print(": ");
              //        lcd.print(temp[i]); lcd.write((char)223); lcd.print("C ");
          //        #endif
          //      }
          //      delay(2000);
        }
        delay(1000);
      }
  #endif
  #if DHT22
      h = dht.readHumidity();       //Gets Humidity value
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.println(" %");
      if (!(isnan(h)))
      {
        #if LCD-I2C
            lcd.setCursor(0, 1);
            lcd.print("Hum 1 : ");
            lcd.print(h);
            lcd.print("% ");
        #endif
      }
      delay(1000);
  #endif
  #if SIM800L
      //  ClientRequest();
      if (Serial1.available())
      {
        delay(1000);
        readTextMessage();
      }
      sendAlert();
  #endif
  displayTime();
}

#if SIM800L
    void ClientRequest()
    {
      Serial.println("Sending Text...");
      Serial1.print("AT+CMGF=1\n"); // Set the shield to SMS mode
      delay(1000);
      Serial1.print("AT+CMGS=\"+254721460975\"\n");
      delay(1000);
      SendTextMessage();
    }
    
    void ClientRequestTwo()
    {
      Serial.println("Sending Text...");
      Serial1.print("AT+CMGF=1\n"); // Set the shield to SMS mode
      delay(1000);
      Serial1.print("AT+CMGS=\"+25436184663\"\n");
      delay(1000);
      SendTextMessage();
    }
    
    void SystemCheck()
    {
      Serial.println("Sending Text...");
      Serial1.print("AT+CMGF=1\n"); // Set the shield to SMS mode
      delay(1000);
      Serial1.print("AT+CMGS=\"+254721460975\"\n");
      delay(1000);
      SendTextMessage();
    }
    
    void SendTextMessage()
    {
      for (int i = 0; i < sensors.getDeviceCount(); i++)
      {
        if (temp[i] != -127.00f)
        {
          Serial1.print("Zone ");
          Serial1.print(i + 1);
          Serial1.print(": ");
          Serial1.print(temp[i]);
          //      Serial.write((char)248);
          Serial1.println(" C");
        }
      }
      Serial1.print("Humidity : "); //the content of the message
      Serial1.print(h); //the content of the message
      Serial1.print("%");
      //  Serial1.print("\n");
      delay(1000);
      Serial1.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
      delay(1000);
    }
    
    void readTextMessage()
    {
      // char incomingByte;
      String inputString;
      // delay(1000);   // Serial Buffer
      while (Serial1.available()) {
        inputString = Serial1.readString();
        // inputString += incomingByte;
      }
      delay(500);
    
      Serial.println(inputString);
      inputString.toUpperCase(); // Uppercase the Received Message
    
      //Send text message
      if (inputString.indexOf("REQUEST") > -1)
      {
        ClientRequest();
        // inputString ="";
      }
      if (inputString.indexOf("CHECK") > -1)
      {
        ClientRequestTwo();
        // inputString ="";
      }
      if (inputString.indexOf("READ") > -1)
      {
        SystemCheck();
        // inputString ="";
      }
    
      delay(2000);
      //Delete Messages & Save Memory
      if (inputString.indexOf("OK") == -1)
      {
        Serial1.println("AT+CMGDA=\"DEL ALL\"");
        delay(2000);
      }
      inputString = "";
    }
    void sendAlert() {
      // Get data from the DS3231
      int second, minute, hour, dayOfWeek, day, month, year;
      // retrieve data from DS3231
      readDS3231time(&second, &minute, &hour, &dayOfWeek, &day, &month, &year);
      int rem = minute % 30;
      //  lcd.setCursor(12,0);
      //  lcd.print(rtc.getTimeStr());
      if (rem == 0 && count == 0) {
        //activate relay
        ClientRequest();
        count++;
      }
      else if (rem > 0)
      {
        count = 0;
      }
      if (rem == 5 && countTwo == 0) {
        //activate relay
        ClientRequestTwo();
        countTwo++;
      }
      else if (rem > 5)
      {
        countTwo = 0;
      }
    }
#endif
void setDS3231time(int second, int minute, int hour, int dayOfWeek, int dayOfMonth, int month, int year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(int *second,
                    int *minute,
                    int *hour,
                    int *dayOfWeek,
                    int *day,
                    int *month,
                    int *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *day = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void displayTime()
{
  int second, minute, hour, dayOfWeek, day, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &day, &month, &year);
  // send it to the serial monitor
  Serial.print(hour);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute < 10)
  {
    Serial.print("0");
  }
  Serial.print(minute);
  Serial.print(":");
  if (second < 10)
  {
    Serial.print("0");
  }
  Serial.print(second);
  Serial.print(" ");
  Serial.print(day);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" Day of week: ");
  switch (dayOfWeek) {
    case 1:
      Serial.println("Sunday");
      break;
    case 2:
      Serial.println("Monday");
      break;
    case 3:
      Serial.println("Tuesday");
      break;
    case 4:
      Serial.println("Wednesday");
      break;
    case 5:
      Serial.println("Thursday");
      break;
    case 6:
      Serial.println("Friday");
      break;
    case 7:
      Serial.println("Saturday");
      break;
    default:
      Serial.println(" ");
      break;
  }
}
