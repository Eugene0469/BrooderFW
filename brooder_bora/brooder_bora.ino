#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <DS3231.h>

/*Definitions for enabling code*/
#define ENABLE_DS18B20 1 //enables the DS18B20
#define ENABLE_DHT22   0 //enables the dht22
#define ENABLE_LCD     0 //enables the lcd
#define ENABLE_DS3231  0 //enables the ds3231
#define ENABLE_PWRDET  0 //enables the system to detect when the mains power is on and off

/*Definitions for both the DHT22 Sensors */
#define ENABLE_DHT1 1 //enables the first dht22 sensor
#define ENABLE_DHT2 1 //enables the second dht22 sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTPIN_1 7     //pin for the first dht22 sensor
#define DHTPIN_2 8     //pin for the second dht22 sensor

/*Definitions for all the DS18B20 Sensors */ 
#define ONE_WIRE_BUS 2  // Data wire is plugged into port 2 on the Arduino
#define ENABLE_DREAD 1 //enables temperature data reading.
#define ENABLE_AREAD 0 //enables device address reading 

/*Definitions for the DS3231 RTC*/
#define SET_PARAMETERS  0 // when set to 1, one can set the day,date and time
#define DS3231_TESTING  1 // when set to 1, it enables the ds3231TestFunction()

#if ENABLE_PWRDET
  const byte interruptPin_Off = 2; // interrupt pin for when mains power goes off
  const byte interruptPin_On = 3; // interrupt pin for when mains power goes on
#endif

#if ENABLE_DS3231
  DS3231  rtc(SDA, SCL);
#endif
 
#if ENABLE_LCD
  // initialize the library by associating any needed LCD interface pin
  // with the arduino pin number it is connected to
  const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#endif

#if ENABLE_DHT22
  #if ENABLE_DHT1
    //Constants
    DHT dht_1(DHTPIN_1, DHTTYPE); //// Initialize first DHT sensor for normal 16mhz Arduino

    //Variables
    int chk_1;
    float hum_1;  //Stores humidity value
    float temp_1; //Stores temperature value
  #endif

  #if ENABLE_DHT2
   DHT dht_2(DHTPIN_2, DHTTYPE); //// Initialize second DHT sensor for normal 16mhz Arduino
    //Variables
    int chk_2;
    float hum_2;  //Stores humidity value
    float temp_2; //Stores temperature value
  #endif
#endif   

#if ENABLE_DS18B20
  // Setup a oneWire instance to communicate with any OneWire devices
  OneWire oneWire(ONE_WIRE_BUS);
  
  // Pass our oneWire reference to Dallas Temperature.
  DallasTemperature sensors(&oneWire);
  #if ENABLE_AREAD
    // variable to hold device addresses
    DeviceAddress Thermometer;
    int deviceCount = 0;
  #endif
  
  #if ENABLE_DREAD
    // Addresses of 3 DS18B20s.Replace with own addresses
    uint8_t sensor1[8] = { 0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x02, 0xEC };
    uint8_t sensor2[8] = { 0x28, 0x61, 0x64, 0x12, 0x3C, 0x7C, 0x2F, 0x27 };
    uint8_t sensor3[8] = { 0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6 };
  #endif
#endif

void setup(void)
{
  Serial.begin(9600);
  #if ENABLE_LCD
    // set up the LCD's number of columns and rows:
    lcd.begin(20, 4);
  #endif
  #if ENABLE_DS18B20
    sensors.begin();
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
   #endif
   #if ENABLE_PWRDET
    pinMode(interruptPin_Off, INPUT_PULLUP);
    pinMode(interruptPin_On, INPUT_PULLUP);
    /*
      @brief  Power Blackout Detection Unit
        When the mainssupply is on the signal level is HIGH. 
        When there is no mains supply, the signal level is LOW
    */
    attachInterrupt(digitalPinToInterrupt(interruptPin_Off), powerOff , FALLING); // this triggers the interrupt on the falling edge of the signal
    attachInterrupt(digitalPinToInterrupt(interruptPin_On), powerOn , RISING);    // this triggers the interrupt on the rising edge of the signal.
   #endif
}

void loop(void)
{
  #if ENABLE_DS18B20
    ds18b20Sensor();
  #endif 

  #if ENABLE_DHT22
    dht22Sensor();
  #endif 
}

#if ENABLE_DS18B20
  void ds18b20Sensor(void)
  {
    #if ENABLE_DREAD
      sensors.requestTemperatures();
      
      Serial.print("Sensor 1: ");
      printTemperature(sensor1);
      
      Serial.print("Sensor 2: ");
      printTemperature(sensor2);
      
      Serial.print("Sensor 3: ");
      printTemperature(sensor3);
      
      Serial.println();
      delay(1000);
    #endif 
  }
  
  #if ENABLE_DREAD
    void printTemperature(DeviceAddress deviceAddress)
    {
      float tempC = sensors.getTempC(deviceAddress);
      Serial.print(tempC);
      Serial.print((char)176);
      Serial.print("C  |  ");
      Serial.print(DallasTemperature::toFahrenheit(tempC));
      Serial.print((char)176);
      Serial.println("F");
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
        delay(2000);
        //Read data and store it to variables hum and temp
        hum_1 = dht_1.readHumidity();
        temp_1= dht_1.readTemperature();
        //Print temp and humidity values to serial monitor
        Serial.print("Humidity_1: ");
        Serial.print(hum_1);
        Serial.print(" %, Temp_1: ");
        Serial.print(temp_1);
        Serial.println(" Celsius");
        delay(10000); //Delay 2 sec.
      #endif
      
      #if ENABLE_DHT2
        delay(2000);
        //Read data and store it to variables hum and temp
        hum_2 = dht_2.readHumidity();
        temp_2= dht_2.readTemperature();
        //Print temp and humidity values to serial monitor
        Serial.print("Humidity_2: ");
        Serial.print(hum_2);
        Serial.print(" %, Temp_2: ");
        Serial.print(temp_2);
        Serial.println(" Celsius");
        delay(10000); //Delay 2 sec.
      #endif
  }
#endif

#if ENABLE_DS3231
  #if DS3231_TESTING
    void ds3231TestFunction(void)
    {
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
  void powerOff(void)
  {
    //TO DO: ADD FUNCTION TO CALL THE FARMER WHEN POWER IS OFF AND SEND SMS
    Serial.println("Power off interrupt triggered"); 
  }
  void powerOn(void)
  {
    //TO DO: ADD FUNCTION TO CALL THE FARMER WHEN POWER IS ON AND SEND SMS
    Serial.println("Power on interrupt triggered");
  }
#endif
