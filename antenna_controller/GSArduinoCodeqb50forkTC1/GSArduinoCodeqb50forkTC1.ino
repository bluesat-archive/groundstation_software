//#define OP_MODE MANUAL_MODE
/*  BLUEsat/QB50 Groundstation
  Arduino Serial to Actuators
  Main Function and Setup code
  Created: T Nguyen, 5-Oct-2014
  Last Modified: B Luc, 25-April-2015
  QB50 Fork Test Candidate 1 (TC1) by: C McKay - VK2CKP, 13/09/2015  //Added for QB50 Fork
 */
/**
AUTO_MODE:
- Reads in Elevation & Azimuth angles from Serial, then sets actuators to desired angles
 - Format: "[Ele,Azi]"; where Ele & Azi are desired degrees * 1000
- Writes "1" to Serial when actuators are set
*/

#include "PINDEF.H"

#define STRING_BUFFER 100
#define OP_MODE MANUAL_MODE
//#define OP_MODE DEBUG_MODE

/** For MANUAL_MODE - desired Elevation & Azimuth angles*/
#define EL 0.0
#define AZ 0.0


/** For Calibration*/
#define AZI_FULL_VOLTS 3.3  // Voltage at Angle +180deg  - (360 degress in GS232 code) //NOTE: Now referenced to 3.3V regulator on arudino - must use external reference in calls
#define AZI_MIN_VOLTS 0.00:0   // Voltage at Angle -180deg  - Added in QB50 fork (0 degrees in GS232 code)
#define AZI_MAX_ANGLE 372.0   // Max azimuth angle - added in QB50 fork
#define AZI_DZ_ANGLE 0.00   // Angle between 'A' and 'B' (degrees)
#define ELE_MIN_VOLTS 0.71   // Voltage at Angle 0deg
#define ELE_MAX_VOLTS 3.3   // Voltage at Angle +180deg //NOTE: Now referenced to 3.3V regulator on arudino - must use external reference in calls
#define ELE_MAX_ANGLE 179     // Set emperically so that 90deg = actual 90deg
#define ELE_DZ_ANGLE 0.000    // Angle between 'A' and 'B' (degrees) - Added in QB50 fork
#define MAX_COUNTS 1023       // Maximum int returned from analogRead()
#define MAX_VOLTS 5.0         // Maximum voltage read by analogRead()
#define PRECISION 0.4         // Smallest angle increment when reading in (estimate)
#define QB50_PRECISION 3.0    // Tolerable Error in positioning for QB50 control - Added in QB50 fork

//gs232 (automatic mode) variables and flags - QB50 fork only
boolean _gs232WActive = false;  // gs232 W command in process
int _gs232AzElIndex = 0;        // position in gs232 Az El sequence
long _gs232Azimuth = 0;          // gs232 Azimuth value
int _azimuthTemp = 0;        // used for gs232 azimuth decoding  0 = North, 360 = North (as per GS232 standard)
float _newAzimuth = 0.0;         // new azimuth for rotor move  0/360 = south, 180 = Nouth (for north centred rotator)
int _ElTemp = 0;        // used for gs232 elevation decoding  0 = horizontal, 90 = vertical, 180 = inverted 
float _newEl = 0.0;         // new elevation for rotor move  0 = horizontal, 90 = vertical, 180 = inverted
boolean _aziMove = false;   //Azimuth move needed
boolean _elMove = false;    //Elevation move needed
float current_az = 0.0;   // Current Azimuth (0/360 = South, 180 = Nouth) - Antenna points SOUTH when ROTATOR in 0deg/360deg position
float current_el = 0.0;   // Current Elevation (0 = Horizontal, 90 = vertical, 180 = inverted)


void debugMenu(void);
void ctrlOff(void);
double aziDegCount(double countIn);  
void setElevation(double set);
void setAzimuth(double set);
double getAzimuth(void);
double getElevation(void);
void normalMenu();
void commandIn(double set[2]);
int MODE = 1;  // Added in QB50 fork. 1 (HIGH) = QB50 mode, 0 (LOW) = BLUESat mode.


/** Runs on launch/reset */
void setup(){
  Serial.begin(9600);          // Init serial comms at 9600 bit/s:
  pinMode(DOWN_PIN, OUTPUT);
  pinMode(LEFT_PIN, OUTPUT);
  pinMode(RIGHT_PIN, OUTPUT);
  pinMode(UP_PIN, OUTPUT);
  pinMode(QB50_MODE_PIN, INPUT_PULLUP);  // Added for QB50 fork
  ctrlOff();
  analogReference(EXTERNAL);  //Added in QB50 fork to reference 3.3V from on board regulator
  MODE=checkmode();
  current_az = get_az();
  current_el = get_el();
}


/** Loops forever after setup() is run */
void loop(){
  MODE=checkmode();
  switch (MODE) {    //Added in QB50 fork
    case 1:      //Added in QB50 fork
    QB50_Code();  
    break;
   case 0:
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
   break; //Added in QB50 fork to termiante 'case FALSE'
  }
  }  //closes MODE switch statement
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
  Serial.print(eleDegCount(elevation));
  Serial.print(" Azimuth: ");
  Serial.println(aziDegCount(azimuth));
  if (Serial.available()) {     // If there's stuff to read in USB-serial,
    c =(char)Serial.read();     // Read in a char
    switch (c) {
      case 'w':
       Serial.println("Going UP");
         digitalWrite(UP_PIN,ON);    //fixed in qb50 fork by changing pindef.h
      break;
      case 'a':
       Serial.println("Going Left");
         digitalWrite(LEFT_PIN,ON);  //fixed in qb50 fork by changing pindef.h
      break;
      case 's':
       Serial.println("Going Down");
         digitalWrite(DOWN_PIN,ON);  //fixed in qb50 fork by changing pindef.h
      break;  Serial.println("QB50 Mode Enabled");
      case 'd':
       Serial.println("Going Right");
         digitalWrite(RIGHT_PIN,ON);  //fixed in qb50 fork by changing pindef.h
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
  int azimuth = analogRead(AZIMUTH_PIN);
  double current = aziDegCount(azimuth);
  //double current = getAzimuth();
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
  while ((current >= (set - PRECISION/2)) || (current <= (set + PRECISION/2))) {
    digitalWrite(RIGHT_PIN, OFF);
    digitalWrite(LEFT_PIN, OFF);
  }
  while (current == (set)) {
    digitalWrite(RIGHT_PIN, OFF);
    digitalWrite(LEFT_PIN, OFF);
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
  //setElevation(set[0]);
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

int checkmode(){                                //Added in QB50 fork to determine whether QB50/GS232 control code to operate or BLUESat code to operate.
  return (digitalRead(QB50_MODE_PIN));
}

void QB50_Code() {
    if (Serial.available() > 0)
  {
    decodeGS232(Serial.read());
  }

if (_aziMove == true)                        // Automatic Control Routine - Azimuth
  {
    // Serial.println("Az MOVE");  **QB50 debug code**
    if (_newAzimuth >= (current_az + QB50_PRECISION))   //_newAzimuth is greater than Az 
    {
      digitalWrite(LEFT_PIN, LOW);
      digitalWrite(RIGHT_PIN, HIGH);
      current_az = get_az();
    }
    else if (_newAzimuth <= (current_az - QB50_PRECISION))   //_newAzimuth is less than than Az 
    {
      digitalWrite(RIGHT_PIN, LOW);
      digitalWrite(LEFT_PIN, HIGH);
      current_az = get_az();
    }
    else
    {
      digitalWrite(LEFT_PIN, LOW);                //Ensures motor turned off when no movement is meant to occur.
      digitalWrite(RIGHT_PIN, LOW);               //Ensures motor turned off when no movement is meant to occur.
      _aziMove = false;                    //Finishes movement loop. Resets flag.
      // Serial.println("Az STOP");  **QB50 debug code **
    }
  }

if (_elMove == true)                        // Automatic Control Routine - Elevation
  {
    // Serial.println("El MOVE"); ** QB50 debug code **
    if (_newEl >= (current_el + QB50_PRECISION))   //_newEl is greater than El
    {
      digitalWrite(DOWN_PIN, LOW);
      digitalWrite(UP_PIN, HIGH);
      current_el = get_el();
    }
    else if (_newEl <= (current_el - QB50_PRECISION))   //_newEl is less than El
    {
      digitalWrite(UP_PIN, LOW);
      digitalWrite(DOWN_PIN, HIGH);
      current_el = get_el();
    }
    else
    {
      digitalWrite(UP_PIN, LOW);                //Ensures motor turned off when no movement is meant to occur.
      digitalWrite(DOWN_PIN, LOW);               //Ensures motor turned off when no movement is meant to occur.
      // Serial.println("El STOP");   **QB50 debug code **
      _elMove = false;                    //Finishes movement loop. Resets flag.
    }
  }

  
  
}

// decode gs232 commands - QB50 Code
//
void decodeGS232(char character)
{
  switch (character)
  {
    case 'w':  // gs232 W command
    case 'W':
     {
        {
          _gs232WActive = true;
          _gs232AzElIndex = 0;
        }
        break;
      }

    // numeric - azimuth and elevation digits
    case '0':  case '1':   case '2':  case '3':  case '4':
    case '5':  case '6':   case '7':  case '8':  case '9':
      {
        if ( _gs232WActive)
        {
          processAzElNumeric(character);
        }
      }

    default:
      {
        // ignore everything else
      }
  }
}

//
// process az el numeric characters from gs232 W command - QB50 Code
//
void processAzElNumeric(char character)
{
  switch (_gs232AzElIndex)
  {
    case 0: // first azimuth character
      {
        _azimuthTemp = (character - 48) * 100;
        _gs232AzElIndex++;
        break;
      }

    case 1:  //second azimuth character
      {
        _azimuthTemp = _azimuthTemp + (character - 48) * 10;
        _gs232AzElIndex++;
        break;
      }

    case 2: // final azimuth character
      {
        _azimuthTemp = _azimuthTemp + (character - 48);
        _gs232AzElIndex++;

      // set up for rotor move
        _newAzimuth = float(_azimuthTemp);
        if ((_newAzimuth) > 180.0)
        {
          _newAzimuth = _newAzimuth - 180.0;
        }
        else
        {
          _newAzimuth = _newAzimuth + 180.0;
        }
        _aziMove = true;
        break;
      }

    case 3: // first elevation character
      {
        _ElTemp = (character - 48) * 100;
        _gs232AzElIndex++;
        break;
      }

    case 4:  //second elevation character
      {
        _ElTemp = _ElTemp + (character - 48) * 10;
        _gs232AzElIndex++;
        break;
      }

    case 5: // last elevation character
      {
        _gs232AzElIndex++;

        // set up for rotor move
        _ElTemp = _ElTemp + (character - 48);
        _newEl = float(_ElTemp);
        _elMove = true;
        _gs232WActive = false;
        break;
      }

    default:
      {
        // should never get here
      }
  }
}

float get_az()
{
 return (((float)analogRead(AZIMUTH_PIN)/1023.0)*360);
 // return (((float)analogRead(AZIMUTH_PIN)*(AZI_MAX_ANGLE/((AZI_FULL_VOLTS/MAX_COUNTS)-(AZI_MIN_VOLTS/MAX_COUNTS))))); **QB50 new code pending **
      
}

float get_el()
{
 return (((float)analogRead(ELEVATION_PIN)/1023.0)*180);
 // return (((float)analogRead(ELEVATION_PIN)*(ELE_MAX_ANGLE/((ELE_MAX_VOLTS/MAX_COUNTS)-(ELE_MIN_VOLTS/MAX_COUNTS)))));   **QB50 new code pending **
}

