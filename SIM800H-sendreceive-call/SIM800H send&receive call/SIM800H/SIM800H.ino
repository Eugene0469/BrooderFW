 /*             GSM MODULE TEST
  *when you type in 's' the program will invoke 
  *function to SEND AN SMS FROM GSM module.if the 
  *user input is 'c ', the programwill invoke the 
  *function to 
  *     
  */
#include <SoftwareSerial.h> //is necesary for the library!! 

SoftwareSerial sim800l(A2, A3); // create a constructor of SoftwareSerial
char incomingByte; 
String inputString;
void setup()
{
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  sim800l.begin(9600);   // Setting the baud rate of GSM Module  
  while(!sim800l.available()){
   sim800l.println("AT");
   delay(1000); 
   Serial.println("Connecting...");
  }
  Serial.println("Connected!"); 
//  delay(100);
//  sim800l.println("AT");
//  delay(1000);
//  sim800l.println("AT+CFUN=?");
//  delay(5000);
//  //sim800l.println("AT+CFUN?");
//  //delay(5000);
//  sim800l.println("AT+CFUN=1");
//  delay(5000);
  sim800l.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(1000);
  sim800l.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS
  delay(1000);
  sim800l.println("AT+CMGL=\"REC UNREAD\""); // Read Unread Messages
}


void loop()
{
  if (Serial.available()>0) // checks for any data  coming through serial port of arduino.
   switch(Serial.read())   //
  {
    case 's':
      SendTextMessage();
      break;
    case 'c':
      DialVoiceCall();
      break;
  }
  readTextMessage();
}


void SendTextMessage()
{
  Serial.println("Sending Text...");
  sim800l.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
  sim800l.print("AT+CMGS=\"+254721460975\"\r");  
  delay(200);
  sim800l.print("This is a Test text message from SIM800L "); //the content of the message
  sim800l.print("\r"); 
  delay(500);
  sim800l.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  sim800l.println();
  Serial.println("Text Sent.");
   delay(500);
}

void DialVoiceCall()
{
  sim800l.println("ATD+ +254721460975;");//dial the number, must include country code
  delay(100);
  sim800l.println();
}

void readTextMessage(){
//if (sim800l.available())
//   Serial.write(sim800l.read()); // prints the data collected from software serial port to serial monitor of arduino
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
  delay(2000);
  //Delete Messages & Save Memory
  if (inputString.indexOf("OK") == -1){
  sim800l.println("AT+CMGDA=\"DEL ALL\"");

  delay(1000);}

  inputString = "";   

}
 
