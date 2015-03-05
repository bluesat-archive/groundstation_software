/*
  BLUEsat Groundstation
  Arduino Serial to Actuators
  Main Function and Setup code
  Created: T Nguyen, 5-Oct-2014
  Last Modified: M Yeo, 23-Oct-2014  
*/
/**
AUTO_MODE:
- Reads in Elevation & Azimuth angles from Serial, then sets actuators to desired angles
 - Format: "[Ele,Azi]"; where Ele & Azi are desired degrees * 1000
- Writes "1" to Serial when actuators are set
*/

#include "PINDEF.H"

#define STRING_BUFFER 100
#define OP_MODE AUTO_MODE

/** For MANUAL_MODE - desired Elevation & Azimuth angles*/
#define EL 20
#define AZ -90


/** For Calibration*/
#define AZI_FULL_VOLTS 5.043  // Voltage at Angle +180deg
#define AZI_DZ_ANGLE 18.783   // Angle between 'A' and 'B' (degrees)
#define ELE_MIN_VOLTS 0.802   // Voltage at Angle +180deg
#define ELE_MAX_VOLTS 4.997   // Voltage at Angle +180deg //NOTE: THIS CHANGES WHEN ARDUINO IS POWERED BY COMPUTER
//#define ELE_MAX_ANGLE 162.28// Angle between 'A' and 'B' (degrees)
#define ELE_MAX_ANGLE 173     // Set emperically so that 90deg = actual 90deg
#define MAX_COUNTS 1023       // Maximum int returned from analogRead()
#define MAX_VOLTS 5.0         // Maximum voltage read by analogRead()
#define PRECISION 0.4         // Smallest angle increment when reading in (estimate)


void debugMenu(void);
void ctrlOff(void);
double aziDegCount(double countIn);
void setElevation(double set);
void setAzimuth(double set);
double getAzimuth(void);
double getElevation(void);
void normalMenu();
void commandIn(double set[2]);



/** Runs on launch/reset */
void setup(){
  Serial.begin(9600);          // Init serial comms at 9600 bit/s:
  pinMode(DOWN_PIN, OUTPUT);
  pinMode(LEFT_PIN, OUTPUT);
  pinMode(RIGHT_PIN, OUTPUT);
  pinMode(UP_PIN, OUTPUT);
  ctrlOff();
}


/** Loops forever after setup() is run */
void loop(){
  switch (OP_MODE){
    case DEBUG_MODE:
      debugMenu();
      break;
    case MANUAL_MODE:
      setElevation(EL);
      setAzimuth(AZ);
      break;
    case AUTO_MODE:
      normalMenu();
      break;
    default:
      break;  
  }
}


/** Turns all control pins off*/
void ctrlOff(void){
  digitalWrite(UP_PIN, OFF);
  digitalWrite(DOWN_PIN, OFF);
  digitalWrite(LEFT_PIN, OFF);
  digitalWrite(RIGHT_PIN, OFF);
}


/** Debug Mode - WASD control of actuators & constant print of feedback;*/
void debugMenu(){
  char c;
  char feedbackString[STRING_BUFFER];
  int elevation = analogRead(ELEVATION_PIN);
  int azimuth = analogRead(AZIMUTH_PIN);
  Serial.print("Elevation: ");
  Serial.print(aziDegCount(elevation));
  Serial.print(" Azimuth: ");
  Serial.println(eleDegCount(azimuth));
  if (Serial.available()) {     // If there's stuff to read in USB-serial,
    c =(char)Serial.read();     // Read in a char
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


/** Converts analogRead() to Elevation degrees */
double eleDegCount(double countIn){
  double voltage = countIn*MAX_VOLTS/MAX_COUNTS;
  double m = (ELE_MAX_ANGLE-0)/(ELE_MAX_VOLTS-ELE_MIN_VOLTS);
  double b = -ELE_MIN_VOLTS*m;
  return m * voltage + b;
}


/** Converts analogRead() to Azimuth degrees */
double aziDegCount(double countIn){
  double voltage = countIn*MAX_VOLTS/MAX_COUNTS;
  double m = (360 - AZI_DZ_ANGLE)/AZI_FULL_VOLTS;
  double b = -180 + AZI_DZ_ANGLE;
  return m * voltage + b;
}


/** Sets elevation actuator to given degrees [0,ELE_MAX_ANGLE] */
void setElevation(double set){
  double current = getElevation();
  if (current > set + PRECISION/2) {        // If the current (actuator) angle > desired angle (set),
    while (current > set + PRECISION/2) {   // Turn the actuator downwards until current angle < desired angle
      digitalWrite(DOWN_PIN, ON);
      //Serial.print("down ");
      //Serial.print(current);
      //Serial.println(set);
      current = getElevation();
    }
  } else if (current < set - PRECISION/2) {
    while (current < set - PRECISION/2) { 
      digitalWrite(UP_PIN, ON);
      //Serial.print("up ");
      //Serial.print(current);
      //Serial.println(set);
      current = getElevation();
    }
  }
  ctrlOff();
}


/** Sets azimuth actuator to given degrees [-180+AZI_DZ_ANGLE,180]*/
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


/** Returns the current (actuator) Elevation angle */
double getElevation(void){
  return eleDegCount(analogRead(ELEVATION_PIN));
}


/** Returns the current (actuator) Azimuth angle */
double getAzimuth(void){
  return aziDegCount(analogRead(AZIMUTH_PIN)); 
}


/** Sets actuators, given E&A angles through Serial */
void normalMenu(){
  double set[2] = {0,0};
  commandIn(set);
  setElevation(set[0]);
  setAzimuth(set[1]);
  Serial.println("1");    // Returns "1" when actuators are set
}

/** Reads in from Serial & stores E&A angles in set[2]
    Format: "[Ele,Azi]", where Ele & Azi are desired angles * 1000 */
void commandIn(double set[2]){
  while(Serial.available() == 0){  // Waits for stuff to be sent to Arduino
    delay(500);
  }
  long temp = 0;
  temp = Serial.parseInt();        // Reads in an int, ignores all previous non-int characters
  set[0] = temp / 1000.0;
  Serial.println(set[0], DEC);
  temp = Serial.parseInt();
  set[1] = temp / 1000.0;
  Serial.println(set[1], DEC);
  temp = Serial.read();            // Gets rid of final ']'
}

