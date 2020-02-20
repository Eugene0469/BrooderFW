#include <DHT.h>                    //Library for using DHT sensor 

#define DHTPIN PB12 

#define DHTTYPE DHT22
#include <DS3231.h>


DHT dht(DHTPIN, DHTTYPE);     //initilize object dht for class DHT with DHT pin with STM32 and DHT type as DHT11
float t,h;
void setup()
{
  // initialize the LCD
  Serial.begin(9600);
  dht.begin();          //Begins to receive Temperature and humidity values.                        
  Serial1.begin(9600);   // Setting the baud rate of GSM Module  
  while(!Serial1.available()){
   Serial1.println("AT");
   delay(1000); 
   Serial.println("Connecting...");
  }
  Serial.println("Connected!"); 
  Serial1.println("AT+CMGDA=\"DEL ALL\"");
  delay(2000);
  Serial1.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(1000);
  Serial1.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS
  delay(1000);
  Serial1.println("AT+CMGL=\"REC UNREAD\""); // Read Unread Messages
  delay(2000);

}

void loop()
{
  h = dht.readHumidity();       //Gets Humidity value
  t = dht.readTemperature();    //Gets Temperature value
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  delay(1000);
  ClientRequest();
  if(Serial1.available())
  {
    delay(1000);
    readTextMessage();
  }
}

void ClientRequest()
{
  Serial.println("Sending Text...");
  Serial1.print("AT+CMGF=1\n"); // Set the shield to SMS mode
  delay(1000);
  Serial1.print("AT+CMGS=\"+254753653176\"\n");  
  delay(1000);
  SendTextMessage(); 
}

void SendTextMessage()
{
  Serial1.print("Humidity : "); //the content of the message
  Serial1.print(h); //the content of the message
  Serial1.println("%");
  Serial1.print("Temperature : "); //the content of the message
  Serial1.print(t); //the content of the message     
  Serial1.println("C");
  Serial1.print("\n"); 
  delay(1000);
  Serial1.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(1000);
}

void readTextMessage()
{
    // char incomingByte; 
    String inputString;
    // delay(1000);   // Serial Buffer
    while(Serial1.available()){
    inputString = Serial1.readString();
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

   delay(2000);
   //Delete Messages & Save Memory
   if (inputString.indexOf("OK") == -1)
  {
   Serial1.println("AT+CMGDA=\"DEL ALL\"");
   delay(2000);
  }
  inputString =""; 
}
