#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <DS3231.h>
#include <SoftwareSerial.h> 

#define ENABLE_DS18B20 1 //enables the DS18B20
#define ENABLE_DHT22   1 //enables the dht22
#define ENABLE_LCD     1 //enables the lcd
#define ENABLE_DS3231  1 //enables the ds3231
#define ENABLE_PWRDET  1 //enables the system to detect when the mains power is on and off
#define ENABLE_SIM800  1 
#define ENABLE_DIM     1

/*Definitions for both the DHT22 Sensors */
#define ENABLE_DHT1 1 //enables the first dht22 sensor
#define ENABLE_DHT2 1 //enables the second dht22 sensor
#define DHTTYPE     DHT22   // DHT 22  (AM2302)
#define DHTPIN_1 6     //pin for the first dht22 sensor
#define DHTPIN_2 7     //pin for the second dht22 sensor

/*Definitions for all the DS18B20 Sensors */ 
//#define ONE_WIRE_BUS   8 // Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS_2 9 // Data wire is plugged into port 2 on the Arduino
#define ENABLE_DREAD   1 //enables temperature data reading.
#define ENABLE_AREAD   0 //enables device address reading 

/*Definitions for the DS3231 RTC*/
#define SET_PARAMETERS  0 // when set to 1, one can set the day,date and time
#define DS3231_TESTING  0 // when set to 1, it enables the ds3231TestFunction()

#if ENABLE_DIM
  int AC_LOAD = 17;    // Output to Opto Triac pin
  int dimming = 128;  // Dimming level (0-128)  0 = ON, 128 = OFF
  int interruptPin = 18;
#endif

#if ENABLE_SIM800
  SoftwareSerial sim800l(2, 3); // create a constructor of SoftwareSerial
#endif
#if ENABLE_PWRDET
  const byte powerPin = 35; // interrupt pin for when mains power goes off
  int prevState;
  int currState;
  void powerDetection();
#endif

#if ENABLE_DS3231
  DS3231  rtc(SDA, SCL);
  int Hor;
  int Min;
  int Sec;
  int Month;
  int Mwaka;
  int Siku;
  Time t;
  int Count=1;
  void getTimeDate();
  void detReference();
#endif
 
#if ENABLE_LCD
  // initialize the library by associating any needed LCD interface pin
  // with the arduino pin number it is connected to
   const int rs = 23, en = 25, d4 = 27, d5 = 29, d6 = 31, d7 = 33;
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#endif

#if ENABLE_DHT22
  #if ENABLE_DHT1
    //Constants
    DHT dht_1(DHTPIN_1, DHTTYPE); //// Initialize first DHT sensor for normal 16mhz Arduino

    //Variables
    int chk_1;
    float hum[2];  //Stores humidity value
    float aveHum; //Stores temperature value
  #endif

  #if ENABLE_DHT2
   DHT dht_2(DHTPIN_2, DHTTYPE); //// Initialize second DHT sensor for normal 16mhz Arduino
    //Variables
    int chk_2;
    float hum_2;  //Stores humidity value
    float temp_2; //Stores temperature value
  #endif
  void dht22Sensor();
#endif   

#if ENABLE_DS18B20
  // Setup a oneWire instance to communicate with any OneWire devices
//  OneWire oneWire(ONE_WIRE_BUS);
  OneWire oneWire_2(ONE_WIRE_BUS_2);
  
  // Pass our oneWire reference to Dallas Temperature.
//  DallasTemperature sensors(&oneWire);
  DallasTemperature sensors_2(&oneWire_2);
  #if ENABLE_AREAD
    // variable to hold device addresses
    DeviceAddress Thermometer;
    int deviceCount = 0;
  #endif
  #if ENABLE_DREAD
    // Addresses of 3 DS18B20s.Replace with own addresses
    uint8_t sensor1[8] = { 0x28, 0x51, 0x7C, 0x77, 0x91, 0x10, 0x02, 0xB6 };
    uint8_t sensor2[8] = { 0x28, 0x4B, 0xCA, 0x77, 0x91, 0x11, 0x02, 0x35 };
    uint8_t sensor3[8] = { 0x28, 0xBD, 0x98, 0x77, 0x91, 0x11, 0x02, 0xE3 };
    uint8_t sensor4[8] = { 0x28, 0x0B, 0x56, 0x77, 0x91, 0x10, 0x02, 0x12 };

    float tempC [4];
    float aveTemp=0;
  
    int TempOn=32;
    int TempOff=36;
  #endif
#endif
#if ENABLE_DIM
  void zero_crosss_int();
#endif
void setup(void)
{
  Serial.begin(9600);
  #if ENABLE_LCD
    // set up the LCD's number of columns and rows:
    lcd.begin(20, 4);
  #endif
  #if ENABLE_DS18B20
//    sensors.begin();
    sensors_2.begin();
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
   #endif

   #if ENABLE_DHT22
    #if ENABLE_DHT1
      dht_1.begin();
    #endif

    #if ENABLE_DHT2
      dht_2.begin();
    #endif
   #endif
   #if ENABLE_DS3231
    rtc.begin(); // Initialize the rtc object
    #if SET_PARAMETERS
      // The following lines can be uncommented to set the date and time
      rtc.setDOW(SATURDAY);     // Set Day-of-Week to SUNDAY
      rtc.setTime(19, 34, 50);     // Set the time to 12:00:00 (24hr format)
      rtc.setDate(24, 8, 2019);   // Set the date to January 1st, 2014
      Serial.println("RTC Configuration Successful");
    #endif
   #endif
   #if ENABLE_PWRDET
    pinMode(powerPin, INPUT_PULLUP);
    prevState = digitalRead(powerPin);
   #endif
   #if ENABLE_SIM800
    sim800l.begin(9600);   // Setting the baud rate of GSM Module  
    delay(100);
    sim800l.println("AT");
    delay(1000);
    sim800l.println("AT+CFUN=?");
    delay(5000);
    //sim800l.println("AT+CFUN?");
    //delay(5000);
    sim800l.println("AT+CFUN=1");
    delay(5000);    
   #endif
   #if ENABLE_DIM
    pinMode(AC_LOAD, OUTPUT);// Set AC Load pin as output
    attachInterrupt(digitalPinToInterrupt(interruptPin), zero_crosss_int, RISING);  // Choose the zero cross interrupt # from the table above
   #endif
}

void loop(void)
{
  #if ENABLE_DS18B20
    ds18b20Sensor();
    regulate();
  #endif 

  #if ENABLE_DHT22
    dht22Sensor();
  #endif 

  #if ENABLE_DS3231
  getTimeDate();
  detReference();
    #if DS3231_TESTING
      ds3231TestFunction();
    #endif
  #endif
  #if ENABLE_PWRDET
    powerDetection();
  #endif
 delay(2000); 
}

#if ENABLE_DS18B20
  void ds18b20Sensor(void)
  {
    #if ENABLE_DREAD
//      sensors.requestTemperatures();
      sensors_2.requestTemperatures();

//      tempC[0] = sensors.getTempC(sensor2);
//      tempC[1] = sensors.getTempC(sensor3);        
//      tempC[2] = sensors.getTempC(sensor4);
      tempC[3] = sensors_2.getTempC(sensor1);
//
//      aveTemp = (tempC[0]+tempC[1]+tempC[2])/3;
      aveTemp = tempC[3];
      Serial.print("Temperature: ");
      Serial.println(aveTemp);
      #if ENABLE_LCD
       lcd.setCursor(0,0);
       lcd.print("Temperature ");
       lcd.print(aveTemp);
      #endif
    #endif 
  }
  
  #if ENABLE_DREAD
    void printTemperature(DeviceAddress deviceAddress, int sensor)
    {
//      tempC[sensor] = sensors.getTempC(deviceAddress);
//      Serial.print(tempC);
//      Serial.print((char)176);
//      Serial.println("C");
//      Serial.print(DallasTemperature::toFahrenheit(tempC));
//      Serial.print((char)176);
//      Serial.println("F");
//      Serial.print("Sensor 1: ");
//      printTemperature(sensor1);
//      #if ENABLE_LCD
//       lcd.setCursor(0,0);
//       lcd.print("Sensor 1: ");
//       printTemperatureLCD();
//      #endif
      
//      Serial.print("Sensor 2: ");
//      printTemperature(sensor2);
//      #if ENABLE_LCD
//       lcd.setCursor(0,1);
//       lcd.print("Sensor 2: ");
//       printTemperatureLCD();
//      #endif
//      
//      Serial.print("Sensor 3: ");
//      printTemperature(sensor3);
//      #if ENABLE_LCD
//        lcd.setCursor(0,2);
//        lcd.print("Sensor 3: ");
//        printTemperatureLCD();
//      #endif
//
//      Serial.print("Sensor 4: ");
//      printTemperature(sensor4);
//      #if ENABLE_LCD
//       lcd.setCursor(0,3);
//       lcd.print("Sensor 4: ");
//       printTemperatureLCD();
//      #endif
//      
//      Serial.println();
    }

  void regulate()
  {
//    if(aveTemp<TempOn)
//    {
//       dimming = 20;
//    }
//    else if (aveTemp>= TempOn &&  aveTemp<=TempOff)
//    {
//      dimming = map(aveTemp,20,120,TempOn,TempOff);
//    }
//    else if(aveTemp>TempOff)
//    {
//      dimming = 126;
//    } 
Serial.println("Nmefika.");

    dimming=60;


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
#endif

#if ENABLE_DHT22
  void dht22Sensor(void)
  {
      #if ENABLE_DHT1
        //Read data and store it to variables hum and temp
        hum[0] = dht_1.readHumidity();
      #endif
      #if ENABLE_DHT2
        //Read data and store it to variables hum and temp
        hum[1] = dht_2.readHumidity();
      #endif    
      aveHum = (hum[0]+hum[1])/2;  
     //Print temp and humidity values to serial monitor
      Serial.print("Humidity: ");
      Serial.print(aveHum);
      Serial.println(" %");
      #if ENABLE_LCD
        lcd.setCursor(0,1);
        lcd.print("Humidity: ");
        lcd.print(aveHum);
        lcd.print("%");
      #endif        
  }
#endif

#if ENABLE_DS3231
  void getTimeDate()
  { 
    t= rtc.getTime();
    Hor=t.hour;
    Min=t.min;
    Sec=t.sec;
    Month=t.mon;
    Mwaka= t.year;
    Siku=t.date;
    // Send Day-of-Week
    Serial.print(rtc.getDOWStr());
    Serial.print(" ");
    
    //Print date
    Serial.print(rtc.getDateStr());
    Serial.print(" -- ");
    //Print time
    Serial.println(rtc.getTimeStr());
  }
  void detReference()
  {
  
     if(Siku==24 && Hor==21 && Min==05 && Count==1)
     {
      TempOn=TempOn-3;
      TempOff=TempOff-3;
      Serial.println(TempOn);
      Serial.println(TempOff);
      Count=2;
     }
    if(Siku==24 && Hor==21 && Min==07 && Count==2)
    {
      TempOn=TempOn-3;
      TempOff=TempOff-3;
      Serial.println(TempOn);
      Serial.println(TempOff);
      Count=3;
    }
    if(Siku==24 && Hor==21 && Min==10 &&Count==3)
    {
      TempOn=TempOn-3;
      TempOff=TempOff-3;
      Serial.println(TempOn);
      Serial.println(TempOff);
      Count=4;
    }
    if(Siku==24 && Hor==21 && Min==11 &&Count==4 )
    {
      TempOn=TempOn-3;
      TempOff=TempOff-3;
      Serial.println(TempOn);
      Serial.println(TempOff);
      Count=5;
    }
  }
  #if DS3231_TESTING
    void ds3231TestFunction(void)
    {
      // Send Day-of-Week
      Serial.print(rtc.getDOWStr());
      Serial.print(" ");
      
      // Send date
      Serial.print(rtc.getDateStr());
      Serial.print(" -- ");
    
      // Send time
      Serial.println(rtc.getTimeStr());
      
    
      
      // Wait one second before repeating
      #if ENABLE_LCD
        lcd.setCursor(0,0);
        lcd.print("Time:  ");
        lcd.print(rtc.getTimeStr());
       
        lcd.setCursor(0,1);
        lcd.print("Date: ");
        lcd.print(rtc.getDateStr());
      #endif
      delay(1000);
    }
  #endif
#endif

#if ENABLE_PWRDET
  void powerDetection(void)
  {
    //TO DO: ADD FUNCTION TO CALL THE FARMER WHEN POWER IS OFF AND SEND SMS
    currState = digitalRead(powerPin);
    if(currState == LOW)
    {
      if(currState != prevState || prevState==LOW)
      {
//        int netStatus = sim800l.print("AT+CREG?");
//         while(sim800l.available())
//         {Serial.write(sim800l.read());}
//        while(netStatus!=1)
//        {
//          netStatus = sim800l.print("AT+CREG?");
//         while(sim800l.available())
//         {Serial.write(sim800l.read());}         
//        }
//        Serial.println("Network Found");
        int count=0;
        do
        {
          Serial.println("Power OFF");
          Serial.println("Sending Text...");
          sim800l.print("AT+CMGF=1\r"); // Set the shield to SMS mode
          delay(100);
          sim800l.print("AT+CMGS=\"+254736184663\"\r");  
          delay(200);
          sim800l.print("Stima imepotea "); //the content of the message
          sim800l.print("\r"); 
          delay(500);
          sim800l.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
          delay(100);
          sim800l.println();
          Serial.println("Text Sent.");
          delay(5000);
          
          sim800l.println("ATD+ +254736184663;");//dial the number, must include country code
//          delay(20000);
//          sim800l.println("ATH");
          count++;
        }while(count<3);
        prevState = currState;  
      }
    } 
    else if (currState == HIGH)
    {
      if(currState != prevState)
      {
        int count=0;
        do
        {
          Serial.println("Power ON");
          Serial.println("Sending Text...");
          sim800l.print("AT+CMGF=1\r"); // Set the shield to SMS mode
          delay(100);
          sim800l.print("AT+CMGS=\"+254736184663\"\r");  
          delay(200);
          sim800l.print("Stima imerudi "); //the content of the message
          sim800l.print("\r"); 
          delay(500);
          sim800l.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
          delay(100);
          sim800l.println();
          Serial.println("Text Sent.");
          delay(5000);
          sim800l.println("ATD+ +254736184663;");//dial the number, must include country code
          delay(100);
          sim800l.println();
          count++;
        }while(count<3);
        prevState = currState;
      }
    }
  }
#endif
#if ENABLE_DIM
  //the interrupt function must take no parameters and return nothing
  void zero_crosss_int()  //function to be fired at the zero crossing to dim the light
  {
    // Firing angle calculation : 1 full 50Hz wave =1/50=20ms 
    // Every zerocrossing thus: (50Hz)-> 10ms (1/2 Cycle) 
    // For 60Hz => 8.33ms (10.000/120)
    // 10ms=10000us
    // (10000us - 10us) / 128 = 75 (Approx) For 60Hz =>65
  
    int dimtime = (75*dimming);    // For 60Hz =>65    
    delayMicroseconds(dimtime);    // Wait till firing the TRIAC    
    digitalWrite(AC_LOAD, HIGH);   // Fire the TRIAC
    delayMicroseconds(10);         // triac On propogation delay 
           // (for 60Hz use 8.33) Some Triacs need a longer period
    digitalWrite(AC_LOAD, LOW);    // No longer trigger the TRIAC (the next zero crossing will swith it off) TRIAC
  }
#endif
