/*
  BLUEsat Groundstation
  Arduino Code
  Main Function and setup code
  Created: T Nguyen, 5-Oct-2014
  Last Modified: T Nguyen, 5-Oct-2014  
*/

#include "PINDEF.h"

#define STRING_BUFFER 50
#define OP_MODE DEBUG_MODE

void debugMenu(void);
void ctrlOff(void);

// the setup routine runs once when you press reset:
void setup() 
{
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(DOWN_PIN, OUTPUT);
  pinMode(LEFT_PIN, OUTPUT);
  pinMode(RIGHT_PIN, OUTPUT);
  pinMode(UP_PIN, OUTPUT);
  
  
  ctrlOff();
  
}

// the loop routine runs over and over again forever:
void loop() 
{
  
  switch (OP_MODE) 
  {
    case DEBUG_MODE:
      debugMenu();
    break;  
  }  
  //Serial.println("Hello World");
}

/*
  turn all control pins off
*/
void ctrlOff(void)
{
  digitalWrite(UP_PIN, OFF);
  digitalWrite(DOWN_PIN, OFF);
  digitalWrite(LEFT_PIN, OFF);
  digitalWrite(RIGHT_PIN, OFF);
}

/*
  Debug Mode - WASD control of the antenna and constant print of feedback;
*/
void debugMenu()
{
  char c;
  char feedbackString[STRING_BUFFER];
  int elevation, azimuth;
  
  elevation = analogRead(ELEVATION_PIN);
  azimuth = analogRead(AZIMUTH_PIN);
  
  sprintf(feedbackString, "Elevation: %04d, Azimuth %04d", elevation, azimuth);
  Serial.println(feedbackString); 
  
  if (Serial.available()) {           // got anything from USB-Serial?
    c =(char)Serial.read();     // read from USB-serial
    switch (c) {
      case 'w':
       Serial.println("Going UP");
       digitalWrite(UP_PIN,ON);
      break;
      case 'a':
       Serial.println("Going Left");
       digitalWrite(LEFT_PIN,ON);
      break;
      case 's':
       Serial.println("Going UP");
       digitalWrite(DOWN_PIN,ON);
      break;
      case 'd':
       Serial.println("Going UP");
       digitalWrite(RIGHT_PIN,ON);
      break;
      default:
        ctrlOff();
      break;
    }
  }
  
}


