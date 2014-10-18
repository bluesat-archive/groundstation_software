/*
  BLUEsat Groundstation
  Arduino Code
  Main Function and setup code
  Created: T Nguyen, 5-Oct-2014
  Last Modified: M Yeo, 13-Oct-2014  
*/

#include "PINDEF.h"

#define STRING_BUFFER 100
//#define OP_MODE DEBUG_MODE
#define OP_MODE NORMAL_MODE
#define FULL_VOLTS 1.599 // Voltage at Angle=180deg
#define DZ_ANGLE 18.783  // Angle between 'A' and 'B' (degrees)
#define MAX_COUNTS 1023
#define MAX_VOLTS 5.0

#define AZ -90
#define EL 20 //desired AZ and EL; for testing purposes only


void debugMenu(void);
void ctrlOff(void);
double degCount(double countIn);
void setElevation(double set);
void setAzimuth(double set);
double getAzimuth(void);
double getElevation(void);


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
  switch (NORMAL_MODE) 
  {
    case DEBUG_MODE:
      debugMenu();
      break;
    case NORMAL_MODE:
      //setElevation(EL);
      setAzimuth(AZ);
      break;
    default:
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
  /*
  sprintf(feedbackString, "Elevation: %4lf, Azimuth %4lf", x, x);//degVolt(aziVolt));//elevation, azimuth);//degVolt(eleVolt)
  Serial.println(feedbackString); 
  */
  Serial.print("Elevation: ");
  Serial.print(degCount(elevation));
  Serial.print(" Azimuth: ");
  Serial.println(degCount(azimuth));
  
  
  
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


double degCount(double countIn){
  double voltage = countIn*MAX_VOLTS/MAX_COUNTS;
  double m = (360 - DZ_ANGLE)/FULL_VOLTS;
  double b = -180 + DZ_ANGLE;
  return m * voltage + b;
}


//Sets elevation actuator to set degrees [-180,180]
void setElevation(double set){
  double current = getElevation();
  if (current > set + 0.5) {
    while (current > set + 0.5) {
      digitalWrite(DOWN_PIN, ON);
      current = getElevation();
    }
  } else if (current < set - 0.5) {
    while (current < set - 0.5) { 
      digitalWrite(UP_PIN, ON);
      current = getElevation();
    }
  }
  ctrlOff();
}

//Sets azimuth actuator to set degrees [-180,180]
void setAzimuth(double set){
  double current = getAzimuth();
  if (current > set + 0.5) {
    while (current > set + 0.5) {
      digitalWrite(LEFT_PIN, ON);
      current = getAzimuth();
      Serial.print("left");
  
      Serial.print(current);
      Serial.println(set);
    }
  } else if (current < set - 0.5) {
    while (current < set - 0.5) { 
      digitalWrite(RIGHT_PIN, ON);
      current = getAzimuth();
      Serial.print("right");
      Serial.print(current);
      Serial.println(set);
    }
  }
  ctrlOff();
}


double getAzimuth(void){
  return degCount(analogRead(AZIMUTH_PIN)); 
}

double getElevation(void){
  return degCount(analogRead(ELEVATION_PIN));
}

