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
#define OP_MODE AUTO_MODE
#define FULL_VOLTS 5.043 // Voltage at Angle +180deg
#define DZ_ANGLE 18.783  // Angle between 'A' and 'B' (degrees)
#define MAX_COUNTS 1023
#define MAX_VOLTS 5.0
#define PRECISION 0.4// Smallest angle increment when reading in

//For MANUAL_MODE - set Azi & Ele to these an (degrees)
#define AZ -90
#define EL 20

void debugMenu(void);
void ctrlOff(void);
double degCount(double countIn);
void setElevation(double set);
void setAzimuth(double set);
double getAzimuth(void);
double getElevation(void);
void normalMenu();
void commandIn(double set[2]);



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
    case MANUAL_MODE:
      //setElevation(EL);
      setAzimuth(AZ);
      break;
    case AUTO_MODE:
      normalMenu();
      break;
    default:
      break;  
  }
}

/** turn all control pins off*/
void ctrlOff(void)
{
  digitalWrite(UP_PIN, OFF);
  digitalWrite(DOWN_PIN, OFF);
  digitalWrite(LEFT_PIN, OFF);
  digitalWrite(RIGHT_PIN, OFF);
}



/** Debug Mode - WASD control of the antenna and constant print of feedback;*/
void debugMenu(){
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

/** Converts analogRead() to degrees*/
double degCount(double countIn){
  double voltage = countIn*MAX_VOLTS/MAX_COUNTS;
  double m = (360 - DZ_ANGLE)/FULL_VOLTS;
  double b = -180 + DZ_ANGLE;
  return m * voltage + b;
}


/** Sets elevation actuator to given degrees [-180+DZ_ANGLE,180] [check?]*/
void setElevation(double set){
  double current = getElevation();
  if (current > set + PRECISION/2) {
    while (current > set + PRECISION/2) {
      digitalWrite(DOWN_PIN, ON);
      current = getElevation();
    }
  } else if (current < set - PRECISION/2) {
    while (current < set - PRECISION/2) { 
      digitalWrite(UP_PIN, ON);
      current = getElevation();
    }
  }
  ctrlOff();
}

/** Sets azimuth actuator to given degrees [-180+DZ_ANGLE,180]*/

/** This block is by Mark
void setAzimuth(double set){
  double current = getAzimuth();
  while (current > set + PRECISION/2) {
    digitalWrite(LEFT_PIN, ON);
    Serial.print("left ");
    Serial.print(current);
    Serial.println(set);
    current = getAzimuth();
  }
  while (current < set - PRECISION/2) { 
    digitalWrite(RIGHT_PIN, ON);
    Serial.print("right ");
    Serial.print(current);
    Serial.println(set);
    current = getAzimuth();
  }
  ctrlOff();
}
*/

void setAzimuth(double target){
  double current = getAzimuth();
  int running = 0; // Check for whether a digitalWrite pin is active
  int LR = 0; // Check which way it's turning. -1 is left, 1 is right
  while ((current - target) < PRECISION) {
    current = getAzimuth();
    
    if (!running){
      if (current > target){
        digitalWrite(LEFT_PIN, ON);
        LR = -1;
      } else {
        digitalWrite(RIGHT_PIN, ON);
        LR = 1;
      }
      running = 1;
    }
    
    if
    Serial.print(
    
    
    
  
        while ((current - target) < PRECISION){
        current = getAzimuth();
        
    Serial.print("left ");
    Serial.print(current);
    Serial.println(set);
    current = getAzimuth();
  }
  while (current < set - PRECISION/2) { 
    digitalWrite(RIGHT_PIN, ON);
    Serial.print("right ");
    Serial.print(current);
    Serial.println(set);
    current = getAzimuth();
  }











double getAzimuth(void){
  return degCount(analogRead(AZIMUTH_PIN)); 
}

double getElevation(void){
  return degCount(analogRead(ELEVATION_PIN));
}


void normalMenu(){
  
  
  double set[2] = {0,0};
  commandIn(set);
  
  //setElevation(set[0]);
  setAzimuth(set[1]);
  Serial.println("Actuator set");
}

void commandIn(double set[2]){
  while(Serial.available() == 0){}
  long temp = 0;
  temp = Serial.parseInt();
  set[0] = temp / 1000.0;
  Serial.println(set[0], DEC);
  temp = Serial.parseInt();
  set[1] = temp / 1000.0;
  Serial.println(set[1], DEC);
  temp = Serial.read(); //get rid of final ']'
    /*
    //while (Serial.read() != '['){}
    char temp[20] = {'\0'};
    int tempi = 0;
    //Reads an int
    int i = 0;
    while(temp[i] != '\n'){
      temp[i] = Serial.read();
      i++;
    }
    tempi = atoi(temp);
    Serial.println(tempi, DEC);
//    set[0] = temp / 1000.0
//    temp = (int)Serial.read();
//    set[1] = temp / 1000.0
*/
}

