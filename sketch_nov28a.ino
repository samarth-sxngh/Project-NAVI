/*
 * This software is written by Students of Padre Conceição College of Engineering. 
 * The basic idea of this software is to read audio messages, convert to string of text messages, and take decision.
 * The text are the commands of the Robot that are programmed in this software.
 * Based on the command match the audio IC is given instrutions to play corresponding messages that are stored in the IC
 * Contributors Sailee Phal, Samarth Singh, Sayhan Ali, Bhairavi Gawas, Kushi Anjum
 * No part of this software should be copied 
 */
#include <SPI.h>   // include the library for serial communication for voice module
#include "BluetoothSerial.h"   //This library for connecting to mobile and adding bluetooth name

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)   // BT is bluetooth, Bluetooth enabled
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;       // Accept data from bluetooth in serial communication
/*
Audio PCB        ESP32 port nunber
SCK              18        // How the audio module is connected to ESP 32
MISO not used    19 
MOSI             23
SS               05

*/
//*****Name the PINS & commands for Audio module
#define PU          0x01        // 01 is the code for power up (PU)of audio module
#define CLR_INT     0x04        // 04 is the code to clear the interrupt  audio module
#define PLAY        0x40        // 40 is the code for play
#define FWD         0x48      // 48 is the code to go to next audio file
#define RST         0x03      // 03 is the reset code for audio module 
#define PD          0x07      // Power down code
const int Slaveselect=5;      // GPIO 5 of esp32 is connected to select the audio module 1
const int Slaveselect2=17; // GPIO 17 of esp32 is connected to select the audio module 2
int MaxRecordNo = 13;   // number of recordings in the audio module 1
int MaxRecordNo2 = 10;   // number of recordings in the audio module 2


// Name pins and variables for Ultrasonic distance sensor
#define echoPin 25  // distance sensor ultrasonic sound receive pin
#define trigPin 33  // distance sensor ultrasonic sound transmit pin
long duration;    // time between sound sent to sound received
int distance;     // distance measured
int sensor_distance = 10;  // distance in cm to detect person and play "may i help you", 
int state = LOW;        // by default, no motion detected

// Name of the pins to control LED's
#define ledtop1 16        // GPIO 16 is connected to LED top
#define ledtop2 4         // GPIO 4 is connected to LED top
#define eye1 2          // GPIO 2 is connected to eye LED
#define eye2 15       // GPIO 15 is connected to eye LED

// assign memory to place for variables used and assign initial value

int recordNo = 1;     // last pointer location to messages in memory of audio module
int playNo = 1;       // pointer to play the current message of audio module
int val = 0;            // variable to keep the bluetooth messages received.


//int sensor = 32;       // the pin that the PIR sensor is attached - PIR sensor not used

// Configure the ports of ESP32 for either INPUT or OUTPUT etc
// below setup program will run only once
void setup() {
 Serial.begin(9600);  // display of messages on serial monitor for debugging software
 SerialBT.begin("3SBK_Robot");  //Place where you give your robot name , Bluetooth device
 Serial.println("Robot started, now you can pair it with bluetooth!"); // message is printed on serial monitor
 
 // LED PORT
   pinMode(ledtop1, OUTPUT);   // configure as output port for switching ON LED
   pinMode(ledtop2, OUTPUT);    // configure as output port for switching ON LED
   pinMode(eye1, OUTPUT);     // configure as output port for switching ON LED
   pinMode(eye2, OUTPUT);     // configure as output port for switching ON LED
  // pinMode(sensor, INPUT);    // initialize sensor as an input- PIR sensor not used
 
  //Ultrasonic SENSOR PORT
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  
// Below line of code will turn on the LED's 3 times ON & OFF during Robot Booting- only once
for (int i = 0; i <= 3; i++){
digitalWrite(ledtop1, HIGH);   // ON the LED as indicated by name
digitalWrite(ledtop2, HIGH);  // ON the LED as indicated by name
digitalWrite(eye2, HIGH);   // ON the LED as indicated by name
digitalWrite(eye1, HIGH);   // ON the LED as indicated by name
 delay(500);
digitalWrite(ledtop1, LOW);   // OFF the LED as indicated by name
digitalWrite(ledtop2, LOW);   // OFF the LED as indicated by name
digitalWrite(eye2, LOW);    // OFF the LED as indicated by name
digitalWrite(eye1,LOW);   // OFF the LED as indicated by name
delay(500);
}

   // Below set of code is used for configuring SPI serial interface
   // SPI (serial pheripheral interface) is used for communicating with audio module
   
 SPI.begin();               // Start the SPI library commands
  SPI.setBitOrder(LSBFIRST);   // send the serial data with LSB first   
  SPI.setDataMode(SPI_MODE3);   // Audio module uses SPI mode 3 for communication
  pinMode(Slaveselect,OUTPUT);  // selecting audio module 1
  pinMode(Slaveselect2,OUTPUT); // selecting audio module 1
  digitalWrite(Slaveselect2,HIGH);  // High means disable the Module from accidental playing of message
  digitalWrite(Slaveselect,HIGH);   // HIGH means disable the module for accidental playing message

  setrecord ();                 // Reset the Audio module1 and set pointer to first record
  setrecord2 ();                 // Reset the Audio module2 and set pointer to first record
 Serial.println("3SBK- Blind Peoples Assistant Robot is Ready");
   playNo=9;             // "May I help you" message is recorded at serial number 9 in audio module 1, play the message
     memoryadjust ();     // adjust the current record pointer to play pointer
     play ();             // tell the audio module to play the message as pointed by record number.
     playdelay();         // randomly blink the eyes along with the message played.
     playdelay();
     playdelay();
}

// below Loop program will be running in loop all the time

void loop() {
 if(SerialBT.available())   // check if you a new command in received in bluetooth(BT) memory

  {
  String val = SerialBT.readString(); // if BT message is available then read the message and store in the variable val

    Serial.println(val);      // print the message so that we can check if the message on screen
  
 if(val == "please call police" || val == "call police" )   // compare the received BT message and take decision
    {
     playNo=1;        // if the message is call police then play record no 1 in module 1
     memoryadjust ();   // adjust the record number to the play number
     play ();         // play the record that is calling police station
   playdelay();     // blink the eyes when  it is playing
   delay(2000);
   playNo=4;        // play start dialling tone message stored at 4
   memoryadjust ();
   play ();
   playdelay();
   delay(3000);
   playNo=5;      // play police responding message stored at 5
   memoryadjust ();
   play ();
   playdelay();
   delay(2000);
 //  powerdown();
    }
    
  else if(val == "please call ambulance" || val == "call ambulance")   // if not check the message with next state and go on continue to check other states.
    {
     playNo=2;        // play calling ambulance message stored at 2
     memoryadjust ();
     play ();
    playdelay();
    delay(2000);
   playNo=4;        // play  dialling tone stored at 4
   memoryadjust ();
   play ();
   playdelay();
   delay(3000);
   playNo=6;        // ambulance responding stored at 6 
   memoryadjust ();
   play ();
   playdelay();
   delay(2000);
//   powerdown();  
    }
//*****
 else if(val == "please call fire station" || val == "call fire station") 
    {
       playNo=3;      // play calling fire station
     memoryadjust ();
     play ();
    playdelay(); 
    delay(2000); 
    playNo=4;        // play start dialling
   memoryadjust ();
   play ();
   playdelay();
   delay(3000);
   playNo=7;        // play Fire station Responding
   memoryadjust ();
   play ();
   playdelay();
   delay(2000);
    
    }
 //*****   
//*****
 else if(val == "what is my location" || val == "where am i") 
    {
       playNo=8;      // my location message stored at 8
     memoryadjust ();
     play ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay(); 
 
    }
 //*****   
  //*****
 else if(val =="who are you" || val =="Who are you") 
    {
       playNo=10;      // message 2
     memoryadjust ();
     play ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay();  

    }
 //*****   
 //*****
 else if(val =="how can you help me" || val =="How can you help me" ) 
    {
       playNo=11;      // message 2
     memoryadjust ();
     play ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay(); 

    }
 //*****   
 //*****
 else if(val =="give me direction to nearest school" || val =="School" ) 
    {
       playNo=12;      // message 2
     memoryadjust ();
     play ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay(); 

    }
 //*****   
 //*****
 else if(val == "give me direction to nearest police station" || val == "police station") 
    {
       playNo=13;      // message 2
     memoryadjust ();
     play ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay();  

    }
 //*****  
 //We are using Memory adjust2 and play 2 indicating the second moodule messages are played
 //*****
 else if(val == "give me direction to nearest post office" || val == "post office") 
    {
       playNo=1;      // message 2
     memoryadjust2 ();
     play2 ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay();
   delay(5000);
   delay(5000);
    delay(1000);
   powerdown2();   // for second module reset is required
    }
 //*****   
 //*****
 else if(val =="give me direction to nearest pharmacy" || val == "pharmacy" ) 
    {
       playNo=2;      // message 2
     memoryadjust2 ();
     play2 ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay();
   delay(5000);
   delay(2000);
   powerdown2();   
    }
 //*****   
 //*****
 else if(val =="give me direction to nearest hospital" || val == "Hospital" ) 
    {
       playNo=3;      // message 2
     memoryadjust2 ();
     play2 ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay(); 
   delay(5000);
   delay(5000);
   powerdown2();  
    }
 //*****   
 //*****
 else if(val == "give me direction to beach bus stop" || val == "beach bus stop" || val == "bus stop towards beach") 
    {
       playNo=4;      // message 2
     memoryadjust2 ();
     play2 ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay();
   delay(5000);
   delay(2000);
   powerdown2();   
    }
 //*****   
 //*****
 else if(val =="give me direction to airport bus stop" || val == "airport bus stop" || val == "bus stop towards airport") 
    {
       playNo=5;      // message 2
     memoryadjust2 ();
     play2 ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay();
   delay(5000);
   delay(3000);
   powerdown2();   
    }
 //*****   
 //*****
 else if(val == "give me direction to bus stop" || val == "bus stop") 
    {
       playNo=6;      // message 2
     memoryadjust2 ();
     play2 ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay(); 
   delay(5000); 
   delay(1000); 
   powerdown2(); 
    }
 //*****   
 
 
 //*****
 else if(val =="who is your creator" || val =="who all are your creators") 
    {
       playNo=7;      // message 2
     memoryadjust2 ();
     play2 ();
  //    Serial.println(recordNo);
    // delay(4000);
   playdelay();  
   delay(5000); 
   delay(1000); 
   powerdown2();
    }
 //***** 
 //*********
 else if(val == "light on" || val == "light ON") 
    {
     playNo=9;        // 
     memoryadjust2 ();
     play2 ();
    digitalWrite(ledtop2, HIGH);
   digitalWrite(ledtop1, HIGH);
   digitalWrite(eye2, HIGH);
digitalWrite(eye1, HIGH);
     
  //   Serial.println(recordNo);
   // delay(7000);
     playdelay();  
   powerdown2();
    }
 
 //*********  
 //*********
 else if(val == "light off" || val == "light OFF") 
    {
     playNo=10;        // 
     memoryadjust2 ();
     play2 ();
   digitalWrite(ledtop2, LOW);
   digitalWrite(ledtop1, LOW);
   digitalWrite(eye2, LOW);
digitalWrite(eye1, LOW);
//   Serial.println(recordNo);
   // delay(3000);
    playdelay(); 
    powerdown2();  
    }
 
  //***** if the message is not matching to any command then play the below message
    else
    {
      playNo=8;  // i m not programmed
     memoryadjust2  ();
     play2 ();
   digitalWrite(ledtop1, LOW);
   digitalWrite(ledtop2, LOW);
  Serial.println(recordNo);
 //  delay(2000);
    playdelay(); 
    delay(3000); 
    powerdown2();
    }
  }
pir_sensor();   // check if anybody is near to sensor
}
//**** play delay******randomly blink the eyes

void playdelay()
{
 
  for (int i = 0; i <= 10; i++) 
     {
  digitalWrite(eye1, !digitalRead(eye1));   // turn the LED on (HIGH is the voltage level)
  digitalWrite(eye2, !digitalRead(eye2));
  delay(random(50,300)); 
 
     }
     digitalWrite(eye2, HIGH);
digitalWrite(eye1, HIGH);
}

//**audio module 1***************
//Adjust the audio module pointer to the record number by resetting the module and changing the pointer until it is equal to play record

void memoryadjust (){
  digitalWrite(ledtop1, HIGH);
  digitalWrite(ledtop2, HIGH);
  setrecord ();
  recordNo = 1;
while(recordNo != playNo)
{
 digitalWrite(Slaveselect2,HIGH);  // disable second audio IC
 digitalWrite(Slaveselect,LOW);
 delay(10);
 SPI.transfer(FWD); // forward
 SPI.transfer(0x00); // data byte
 digitalWrite(Slaveselect,HIGH);
 delay(5);
recordNo = recordNo + 1;

if(recordNo > MaxRecordNo)
  {
    recordNo = 1;
  }
  
 
}
 Serial.print("play no =");
  Serial.println(playNo);
    Serial.print("record no =");
  Serial.println(recordNo);
  digitalWrite(ledtop1, LOW);
  digitalWrite(ledtop2, LOW);
}

//**Audio module 2***************
void memoryadjust2 (){
  digitalWrite(ledtop1, HIGH);
  digitalWrite(ledtop2, HIGH);
  setrecord2 ();
  recordNo = 1;
while(recordNo != playNo)
{
 digitalWrite(Slaveselect,HIGH); //disable 1st IC module
 digitalWrite(Slaveselect2,LOW);
 delay(10);
 SPI.transfer(FWD); // forward
 SPI.transfer(0x00); // data byte
 digitalWrite(Slaveselect2,HIGH);
 delay(5);
recordNo = recordNo + 1;

if(recordNo > MaxRecordNo2)
  {
    recordNo = 1;
  }
  
 
}
 Serial.print("play no =");
  Serial.println(playNo);
    Serial.print("record no 2 =");
  Serial.println(recordNo);
  digitalWrite(ledtop1, LOW);
  digitalWrite(ledtop2, LOW);
}
//**audio module 1***************
// Play the record where the pointer pointing
void play (){

  digitalWrite(Slaveselect,LOW);
 SPI.transfer(PLAY); // play
 SPI.transfer(0x00); // data byte
 digitalWrite(Slaveselect,HIGH);
 // delay(1000);

}
//**audio module 2***************
void play2 (){
  digitalWrite(Slaveselect2,LOW);
 SPI.transfer(PLAY); // play
 SPI.transfer(0x00); // data byte
 digitalWrite(Slaveselect2,HIGH);
//  delay(1000);
 
}

//*audio module 1****************
void powerdown(){
    digitalWrite(Slaveselect,LOW);
 SPI.transfer(PD); // power down
 SPI.transfer(0x00); // data byte
 digitalWrite(Slaveselect,HIGH);
delay(1000);
}

//**audio module 2*****************
void powerdown2 (){
  
 digitalWrite(Slaveselect2,LOW);
 delay(10);
  SPI.transfer(RST);          // reset
  SPI.transfer(0x00); 
  digitalWrite(Slaveselect2,HIGH);
  delay(10);
}



//*Audio module 1******************
// reset the module
// then  Powerup the module
// then clear the interrupt
// forward the pointer by once so that it takes the first location from last location

void setrecord (){
  digitalWrite(Slaveselect,LOW);
 delay(10);
  SPI.transfer(RST);          // reset
  SPI.transfer(0x00); 
  digitalWrite(Slaveselect,HIGH);
  delay(10);
    
  digitalWrite(Slaveselect,LOW);
   delay(10);
  SPI.transfer(PU);             // power up
  SPI.transfer(0x00); 
  digitalWrite(Slaveselect,HIGH);
  delay(10);
 
 digitalWrite(Slaveselect,LOW);
  delay(10);
 SPI.transfer(CLR_INT); // clear interupt 
 SPI.transfer(0x00); 
 digitalWrite(Slaveselect,HIGH);
 delay(10);

 digitalWrite(Slaveselect,LOW);
  delay(10);
 SPI.transfer(FWD);           // forward once to first location
 SPI.transfer(0x00); // data byte
 digitalWrite(Slaveselect,HIGH);
 delay(10);
}

//*Audio module 2******************
void setrecord2 (){
digitalWrite(Slaveselect2,LOW);
 delay(10);
  SPI.transfer(RST);          // reset
  SPI.transfer(0x00); 
  digitalWrite(Slaveselect2,HIGH);
  delay(10);
    
  digitalWrite(Slaveselect2,LOW);
   delay(10);
  SPI.transfer(PU);             // power up
  SPI.transfer(0x00); 
  digitalWrite(Slaveselect2,HIGH);
  delay(10);
 
 digitalWrite(Slaveselect2,LOW);
  delay(10);
 SPI.transfer(CLR_INT); // clear interupt 
 SPI.transfer(0x00); 
 digitalWrite(Slaveselect2,HIGH);
 delay(10);

 digitalWrite(Slaveselect2,LOW);
  delay(10);
 SPI.transfer(FWD);           // forward once to first location
 SPI.transfer(0x00); // data byte
 digitalWrite(Slaveselect2,HIGH);
 delay(10);
}

//*****************
void pir_sensor(){
  
  digitalWrite(trigPin, LOW); // Clears the trigPin condition
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);// Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);  // Reads the echoPin, returns the sound wave travel time in microseconds
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("sensor_distance = ");
  Serial.print(sensor_distance);

// Set the distance for visually impaired to reach the robot for which it can say MAY I HELP YOU
// The state = LoW ensures that for one person, only once the announcement comes..
     if (distance <= sensor_distance) {  
    if (state == LOW) {
      Serial.println("Motion detected!"); 
      state = HIGH;       // update variable state to HIGH
      digitalWrite(ledtop2, HIGH); 

      playNo=9;             // Welcome message is recorded at serial number 40 , play the welcome message
     memoryadjust ();     // adjust the record pointer to play pointer
     play ();             // tell the audio module to play the message as pointed by record number.
     playdelay();         // randomly blink the eyes along with the message played.
    // playdelay();
    // playdelay();   
        
      
    }
    }
  
  else {
    
      
      if (state == HIGH){
        Serial.println("Motion stopped!");
        state = LOW;       // update variable state to LOW
        digitalWrite(ledtop2, LOW); // turn LED OFF
    }
  }

}

//*****************
