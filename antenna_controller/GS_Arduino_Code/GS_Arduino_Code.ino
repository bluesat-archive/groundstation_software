/*
  BLUEsat Groundstation
  Arduino Serial to Actuators
  Main Function and Setup code
  Created: T Nguyen, 5-Oct-2014
  Last Modified: B Luc, 7-Dec-2015
  Shoutouts to Cameron for QB50 Implemetation and Mark Yeo for implementing most of the code  
*/
/**
AUTO_MODE:
- Reads in Elevation & Azimuth angles from Serial, then sets actuators to desired angles
 - Format: "[Ele,Azi]"; where Ele & Azi are desired degrees * 1000
- Writes "1" to Serial when actuators are set
*/

#include "PINDEF.H"

#define STRING_BUFFER 100

/** For MANUAL_MODE - desired Elevation & Azimuth angles*/
#define EL 0
#define AZ 0


/** For Calibration*/
#define AZI_FULL_VOLTS 5.043  // Voltage at Angle +180deg
#define AZI_MIN_VOLTS 0.000 //Voltage at Angle -180deg  - Added in QB50 fork (0 degrees in GS232)
#define AZI_DZ_ANGLE 18.783   // Angle between 'A' and 'B' (degrees)
#define ELE_MIN_VOLTS 0.802   // Voltage at Angle +180deg
#define ELE_MAX_VOLTS 4.997   // Voltage at Angle +180deg //NOTE: THIS CHANGES WHEN ARDUINO IS POWERED BY COMPUTER
#define ELE_MAX_ANGLE 173.0 // Angle between 'A' and 'B' (degrees)
#define ELE_MAX_ANGLE 173     // Set emperically so that 90deg = actual 90deg
#define ELE_DZ_ANGLE 0.000    // Angle between 'A' and 'B' Degrees
#define MAX_COUNTS 1023       // Maximum int returned from analogRead()
#define MAX_VOLTS 5.0         // Maximum voltage read by analogRead()
#define PRECISION 0.4         // Smallest angle increment when reading in (estimate)
#define TIME_PRECISION 5      // Number of continuous measurements needed 'in bounds' before actuator stops
#define QB50_PRECISION 3.0 // Tolerable Error in positioning for QB50 control

void debugMenu(void);
void ctrlOff(void);
void ctrlOn(void);
double aziDegCount(double countIn);
void setElevation(double set);
void setAzimuth(double set);
double getAzimuth(void);
double getElevation(void);
void normalMenu();
void commandIn(double set[2]);

// gs232 (automatic mode) variables and flags
boolean _gs232WActive = false;
int _gs232AzElIndex = 0;
long _gs232Azimuth = 0;
int _azimuthTemp = 0;
float _newAzimuth = 0.0;
int _ElTemp = 0;
float _newEl = 0.0;
boolean _aziMove = false;
boolean _elMove = false;
float current_az = 0.0;
float current_el = 0.0;

//#define OP_MODE MANUAL_MODE

/** Runs on launch/reset */
void setup(){
  if (digitalRead(QB50_MODE_PIN)){
    #define OP_MODE QB50_MODE
    current_az = get_az();
    current_el = get_el();          
  }
  else{
    #define OP_MODE AUTO_MODE // If using Python Implementation
    //#define OP_MODE MANUAL_MODE // If using Java Implementation
  }
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
      //1setElevation(EL); 
      setAzimuth(AZ);  
      break;
    case AUTO_MODE:
      normalMenu();
      break;
    case QB50_MODE:
      QB50_Code();
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

/** Turns all control pins on */
void ctrlOn(void){
  digitalWrite(UP_PIN, ON);
  digitalWrite(DOWN_PIN, ON);
  digitalWrite(LEFT_PIN, ON);
  digitalWrite(RIGHT_PIN, ON);
}


/** Debug Mode - WASD control of actuators & constant print of feedback;*/
void debugMenu(){
  char c;
  char feedbackString[STRING_BUFFER];
  int elevation = analogRead(ELEVATION_PIN);
  int azimuth = analogRead(AZIMUTH_PIN);
  Serial.print("Elevation: ");
  Serial.print(eleDegCount(elevation));
  Serial.print(" Azimuth: ");
  Serial.println(aziDegCount(azimuth));
  if (Serial.available()) {     // If there's stuff to read in USB-serial,
    c =(char)Serial.read();     // Read in a char
    switch (c) {
      case 'w':
       Serial.println("Going UP");
         digitalWrite(RIGHT_PIN,ON);
      break;
      case 'a':
       Serial.println("Going Left");
         digitalWrite(DOWN_PIN,ON); //was left
      break;
      case 's':
       Serial.println("Going Down");
         digitalWrite(UP_PIN,ON);
      break;
      case 'd':
       Serial.println("Going Right");
         digitalWrite(LEFT_PIN,ON); //was riGht
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
  int elevation = analogRead(ELEVATION_PIN);
  double current = eleDegCount(elevation);
  //double current = getElevation();
  if (current > set + PRECISION/2) {        // If the current (actuator) angle > desired angle (set),
    while (current > set + PRECISION/2) {   // Turn the actuator downwards until current angle < desired angle
      digitalWrite(UP_PIN, ON);
      //Serial.print("down ");
      //Serial.print(current);
      //Serial.println(set);
      current = getElevation();
    }
  } else if (current < set - PRECISION/2) {
    while (current < set - PRECISION/2) { 
      digitalWrite(RIGHT_PIN, ON);
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
  int azimuth = analogRead(AZIMUTH_PIN);
  int inBoundCount = 0;  //a counter to count how many times we are in the 'precision' bounds of the 'set' value
  double current = aziDegCount(azimuth);
  //double current = getAzimuth();
  while (current > set + PRECISION/2) {
    inBoundCount =  0;
    digitalWrite(DOWN_PIN, ON);
    Serial.print("left ");
    Serial.print(current);
    Serial.println(set);
    current = getAzimuth();
  }
  while (current < set - PRECISION/2) { 
    inBoundCount =  0;
    digitalWrite(LEFT_PIN, ON);
    Serial.print("right ");
    Serial.print(current);
    Serial.println(set);
    current = getAzimuth();
  }
  while ((current >= (set - PRECISION/2)) || (current <= (set + PRECISION/2))) {
    inBoundCount = inBoundCount + 1; //Count how many times we are in the precision zone.
    if (inBoundCount == TIME_PRECISION) { 
      digitalWrite(DOWN_PIN, OFF);
      digitalWrite(LEFT_PIN, OFF);
    }
  }
  while (current == (set)) {
    inBoundCount = inBoundCount + 1; //Count how many times we are in the precision zone. 
    if (inBoundCount == TIME_PRECISION) { 
      digitalWrite(DOWN_PIN, OFF);
      digitalWrite(LEFT_PIN, OFF);
    }
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
  ctrlOn();
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
    delay(1000);
  }
  long temp = 0.0;            
  temp = Serial.parseFloat();        // Reads in an int, ignores all previous non-int characters
  printf("serial is &d",temp);
  set[0] = temp;
  Serial.println(set[0], DEC);
  temp = Serial.parseFloat();
  set[1] = temp;
  Serial.println(set[1], DEC);
  temp = Serial.read();            // Gets rid of final ']'
}


// QB50 Implementation
void QB50_Code() {
   if (Serial.available() > 0){
      decodeGS232(Serial.read());
   } 
   if(_aziMove == true){
     if(_newAzimuth >= (current_az + QB50_PRECISION)){
       digitalWrite(LEFT_PIN, LOW);
       digitalWrite(RIGHT_PIN, HIGH);
       current_az = get_az();
     
     }
     else if(_newAzimuth <= (current_az - QB50_PRECISION)){
       digitalWrite(RIGHT_PIN, LOW);
       digitalWrite(LEFT_PIN, HIGH);
       current_az = get_az();
     }
     else{
       digitalWrite(LEFT_PIN, LOW);
       digitalWrite(RIGHT_PIN, LOW);
       _aziMove = false;
     }
   }
   if(_elMove == true){
     if(_newEl >= (current_el + QB50_PRECISION)){
       digitalWrite(DOWN_PIN, LOW);
       digitalWrite(UP_PIN, HIGH);
       current_el = get_el();
     
     }
     else if(_newEl <= (current_el - QB50_PRECISION)){
       digitalWrite(UP_PIN, LOW);
       digitalWrite(DOWN_PIN, HIGH);
       current_el = get_el();
     }
     else{
       digitalWrite(UP_PIN, LOW);
       digitalWrite(DOWN_PIN, LOW);
       _elMove = false;
     }
   }  
}

// decode gs232 commands - QB50 Code
void decodeGS232(char character){
  switch(character){
    case 'w':
    case 'W':
      {
        {
          _gs232WActive = true;
          _gs232AzElIndex = 0;
        }
        break;
      
      }
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      {
        if(_gs232WActive){
          processAzElNumeric(character);
        }
      }
    
    default:
      {
        //ignore everything else
      }
  }
}

// process az el numeric characters from gs232 W command - QB50 Code
void processAzElNumeric(char character){
  switch(_gs232AzElIndex){
    case 0: //first azi character
      {
        _azimuthTemp = (character-48) * 100;
        _gs232AzElIndex++;
        break;
      }
    case 1: //second azi character
      {
        _azimuthTemp += (character - 48) * 10;
        _gs232AzElIndex++;
      }
    case 2: //final azi character
      {
        _azimuthTemp += (character - 48);
        _gs232AzElIndex++;
        
        // set up for rotor move
        _newAzimuth = float(_azimuthTemp);
        if ((_newAzimuth) > 180.0){
          _newAzimuth -= 180;
        }
        else{
          _newAzimuth += 180;
        }
        _aziMove = true;
        break;
      }
    case 3: //first ele character
      {
        _ElTemp = (character - 48) * 100;
        _gs232AzElIndex++;
        break;
      }
    case 4: //second azi character
      {
        _ElTemp += (character - 48);
        _newEl = float(_ElTemp);
        _elMove = true;
        _gs232WActive = false;
        break;
      }
    case 5: //final ele character
      {
        _gs232AzElIndex++;
        //set up for rotor move
        _ElTemp += (character -48);
        _newEl = float(_ElTemp);
        _elMove = true;
        _gs232WActive = false;
        break;
      }
     default:
       {
         //should never get here
       }
  }
}


float get_az(){
  return (((float)analogRead(AZIMUTH_PIN)/1023.0)*360);
}

float get_el(){
  return (((float)analogRead(ELEVATION_PIN)/1023.0)*180);
}               
