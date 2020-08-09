
#include <Arduino.h>
#include <DHT.h>                    //Library for using DHT sensor 
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Wire.h"
#include <LiquidCrystal_I2C.h>

#define DS18B20_ENABLED     1
#define DHT22_ENABLED       1
#define LCD_I2C_ENABLED     1
#define SIM800L_ENABLED     1
#define RTC_ENABLED         1

#if DS18B20_ENABLED
  #define ONE_WIRE_BUS PB12 // Data wire is plugged into port PB13 on the Blue Pill
  #define precision 12 // OneWire precision Dallas Sensor
  #define NUM_OF_SENSORS 9 //Number of connected sensors
  int sen_number = 0; // Counter of Dallas sensors
  OneWire oneWire(ONE_WIRE_BUS);
  DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
  DeviceAddress T[NUM_OF_SENSORS]; // arrays to hold device addresses
  float temp[NUM_OF_SENSORS] = {0.0f};
  void DS18b20Init();
  void DS18b20ReadTemp();
#endif //DS18B20_ENABLED

#if DHT22_ENABLED
  #define DHTPIN PB13 //dht22 sensor is connected to pin PB13 of the bluepill
  #define DHTTYPE DHT22
  DHT dht(DHTPIN, DHTTYPE);     //initilize object dht for class DHT with DHT pin with STM32 and DHT type as DHT22
  float h = 0.0f; // Variable to hold the value of the r/h measured 
  void DHT22ReadHum();
#endif //DHT22_ENABLED

#if LCD_I2C_ENABLED
  #define LCDSPLITSCREEN_ENABLED 0
  LiquidCrystal_I2C lcd(0x27, 20, 4);
#endif //LCD_I2C_ENABLED

#if RTC_ENABLED
  #define SET_RTC_TIME_ENABLED  0
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
  void displayTime();
  void readDS3231time(int *second,int *minute,int *hour,int *dayOfWeek,int *day,int *month,int *year);
  #if SET_RTC_TIME_ENABLED
      void setDS3231time(int second, int minute, int hour, int dayOfWeek, int dayOfMonth, int month, int year);
  #endif //SET_RTC_TIME_ENABLED
#endif //RTC_ENABLED

#if SIM800L_ENABLED
  #define SIM800_TEST_ENABLED 0  
  void Sim800Init();
  void ClientRequest();
  void ClientRequestTwo();
  void SystemCheck();
  void SendTextMessage();
  void readTextMessage();
  #if RTC_ENABLED
    void sendUpdate();
  #endif //RTC_ENABLED
  #if SIM800_TEST_ENABLED
    void Sim800TestFxn();
  #endif// SIM800_TEST_ENABLED
#endif// SIM800L_ENABLED

void setup()
{
  delay(5000);
  Wire.begin();
  Serial.begin(9600);
  
  #if LCD_I2C_ENABLED
      lcd.begin();
      lcd.backlight();//To Power ON the back light
  #endif //LCD_I2C_ENABLED
  
  #if DHT22_ENABLED
      dht.begin();          //Begins to receive Temperature and humidity values.
  #endif //DHT22_ENABLED
  
  #if DS18B20_ENABLED
    DS18b20Init();
  #endif //DS18B20_ENABLED
  
  #if SIM800L_ENABLED
    Sim800Init();
    #if SIM800_TEST_ENABLED
      Serial.println("Testing SIM800L");
      Sim800TestFxn();
    #endif //SIM800_TEST_ENABLED
  #endif //SIM800L_ENABLED
  
  #if LCD_I2C_ENABLED
      lcd.clear();
  #endif //LCD_I2C_ENABLED
  //  while(Serial1.available())
  //  {
  //    Serial.write(Serial1.read());
  //  }
  #if SET_RTC_TIME_ENABLED
    /*setDS3231time(int second, int minute, int hour, int dayOfWeek, int dayOfMonth, int month, int year(0-99))*/
    setDS3231time(28, 6, 18, 1, 9, 8, 20);
  #endif // SET_RTC_TIME_ENABLED
}

void loop()
{
  #if DS18B20_ENABLED
    DS18b20ReadTemp();
  #endif //DS18B20_ENABLED
  #if DHT22_ENABLED
    DHT22ReadHum();
  #endif //DHT22_ENABLED
  #if SIM800L_ENABLED
      //  ClientRequest();
      if (Serial1.available()) //Listens to the TX line of the MT(mobile terminal) for any messages
      {
        delay(1000);
        readTextMessage(); //Reads the message on the MT's, stores it and displays it on Serial Monitor
      }
      #if RTC_ENABLED
          sendUpdate();
      #endif //RTC_ENABLED
  #endif //SIM800L_ENABLED
  #if RTC_ENABLED
    displayTime();
  #endif //RTC_ENABLED
}
#if DS18B20_ENABLED
  void DS18b20Init()
  {
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
  }
  void DS18b20ReadTemp()
  {
    float temp_local[NUM_OF_SENSORS] = {0.0f};
    sensors.requestTemperatures();
    for (int i = 0; i < sensors.getDeviceCount(); i++)
    {
      temp_local[i] = sensors.getTempC(T[i]);
      if (temp_local[i] != -127.00f)
      {
        temp[i] = temp_local[i];
        Serial.print("Zone ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(temp[i]);
        Serial.println(" C");
        #if LCD_I2C_ENABLED
            lcd.setCursor(0, i);
            lcd.print("Zone ");
            lcd.print(i + 1);
            lcd.print(": ");
            lcd.print(temp[i]); lcd.write((char)223); lcd.print("C ");
        #endif //LCD_I2C_ENABLED
        #if LCDSPLITSCREEN_ENABLED
          i++;
          if(i == sensors.getDeviceCount())break;
          temp[i] = sensors.getTempC(T[i]);
          if(temp[i] != -127.00)
          {
            #if LCD_I2C_ENABLED
              lcd.setCursor(0,1);
              lcd.print("Zone ");
              lcd.print(i+1);
              lcd.print(": ");
              lcd.print(temp[i]); lcd.write((char)223); lcd.print("C ");
            #endif //LCD_I2C_ENABLED
          }
        #endif //LCDSPLITSCREEN_ENABLED
        delay(2000);
      }
      delay(1000);
    }
  }
#endif //DS18B20_ENABLED
#if DHT22_ENABLED
  void DHT22ReadHum()
  {
    float h_local;
    h_local = dht.readHumidity();       //Gets Humidity value
    if (!(isnan(h)))
    {
      h = h_local;
      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.println(" %");
      #if LCD_I2C_ENABLED
          lcd.setCursor(0, 1);
          lcd.print("Hum 1 : ");
          lcd.print(h);
          lcd.print("% ");
      #endif //LCD_I2C_ENABLED
    }
    delay(1000);
  }
#endif //DHT22_ENABLED
#if SIM800L_ENABLED 
  #if SIM800_TEST_ENABLED
    void Sim800TestFxn()
    {
        Serial1.println("AT+CMEE=2"); //Set the ME's result error code
        delay(2000);
        Serial1.println("AT+CPIN?"); //Checks for pin status. Can be used to check if sim is inserted(properly) or not.
        delay(2000);
        Serial1.println("AT+CSQ"); //Returns signal strength indication. Response= +CSQ: <rssi>,<ber>  
        delay(2000);
        Serial1.println("AT+COPS=?"); //Checks for available networks
        delay(2000);
        Serial1.println("AT+CSCS?"); //Checks for terminal equipment's(TE) ch_set. Can be used to check if antennae is ok.
        delay(2000);
    }
  #endif //SIM800_TEST_ENABLED
  void Sim800Init()      //Function to Send Message
  {
    Serial1.begin(9600);   // Setting the baud rate of GSM Module
    while (!Serial1.available()) 
    {
      Serial1.println("AT");
      delay(1000);
      Serial.println("Connecting...");
      #if LCD_I2C_ENABLED
          lcd.setCursor(0, 0);
          lcd.print("Connecting...");
      #endif //LCD_I2C_ENABLED
    }
    Serial.println("Connected!");
    #if LCD_I2C_ENABLED
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connected");
    #endif //LCD_I2C_ENABLED
    Serial1.println("AT+CMGDA=\"DEL ALL\"");
    delay(2000);
    Serial1.println("AT+CMGF=1"); // Configuring TEXT mode
    delay(1000);
    Serial1.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS
    delay(1000);
    Serial1.println("AT+CMGL=\"REC UNREAD\""); // Read Unread Messages
    delay(2000);
    /*Send a message to indicate that the system has restarted*/
    Serial.println("System Restart");
    Serial1.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
    delay(1000);
    Serial1.println("AT+CMGS=\"+254721460975\"\r"); // Replace x with mobile number
    delay(1000);
    Serial1.print("System Restart"); // The SMS text you want to send
    delay(100);
    Serial1.print((char)26);// ASCII code of CTRL+Z. It marks the end of the text message
    delay(1000);
  }
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
    Serial1.print("AT+CMGS=\"+254721460975\"\n");
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
    #if SIM800_TEST_ENABLED
      Serial1.println("Hello World");
    #endif //SIM800_TEST_ENABLED
    #if DS18B20_ENABLED
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
    #endif //DS18B20_ENABLED
    #if DHT22_ENABLED
      Serial1.print("Humidity : "); //the content of the message
      Serial1.print(h); //the content of the message
      Serial1.print("%");
    #endif //DHT22_ENABLED
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
  #if RTC_ENABLED
    void sendUpdate() //Sends an update to the farmer after every x(30) minutes
    {
      static int count = 0, countTwo = 0;
      // Get data from the DS3231
      int second, minute, hour, dayOfWeek, day, month, year;
      // retrieve data from DS3231
      readDS3231time(&second, &minute, &hour, &dayOfWeek, &day, &month, &year);
      int rem = minute % 30;
      //  lcd.setCursor(12,0);
      //  lcd.print(rtc.getTimeStr());
      if (rem == 0 && count == 0) 
      {
        //activate relay
        ClientRequest();
        count++;
      }
      else if (rem > 0)
      {
        count = 0;
      }
      if (rem == 5 && countTwo == 0) 
      {
        //activate relay
        ClientRequestTwo();
        countTwo++;
      }
      else if (rem > 5)
      {
        countTwo = 0;
      }
    }
  #endif //RTC_ENABLED
#endif //SIM800L_ENABLED
#if RTC_ENABLED
  #if SET_RTC_TIME_ENABLED
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
  #endif //SET_RTC_TIME_ENABLED
  void readDS3231time(int *second,int *minute,int *hour,int *dayOfWeek,int *day,int *month,int *year)
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
#endif //RTC_ENABLED
