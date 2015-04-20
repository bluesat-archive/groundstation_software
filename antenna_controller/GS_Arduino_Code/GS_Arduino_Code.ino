//Mark Yeo; mark.yeo@student.unsw.edu.au
//Last Modified: 20Apr15
//Groundstation Code - Controller for Antenna Actuators
// Takes in commands via serial, orients antenna to given angles
// Commands in format: [<Azimuth angle * 1000>, <Elevation angle * 1000>]

enum mode_select {
  WASD_MODE,  // Serial WASD control + current angle feedback
  MANUAL_MODE, // Set actuator angles through EL & AZ constants
  SERIAL_MODE    // Reads desired angles in from Serial; Format: "[Ele,Azi]"; where Ele & Azi are desired degrees * 1000
};

#include "actuator.h"
#define MODE_SELECT SERIAL_MODE
#define MANUAL_AZ -90 //-180deg - 180deg
#define MANUAL_EL 20  //0deg - 180deg



void setup(){
  Serial.begin(9600);
  initEle();
  initAzi();
}


void loop(){
  switch (MODE_SELECT){
    case WASD_MODE:      //For controlling actuators via WASD (any other key to stop)
      wasdMenu();
      break;
    case MANUAL_MODE:    //For setting angles via MANUAL_AZ/EL constants
      setAzi(MANUAL_AZ);
      setEle(MANUAL_EL);
      break;
    case SERIAL_MODE:    //For setting angles via serial (in format: [<Azimuth angle * 1000>, <Elevation angle * 1000>])
      serialMenu();
      break;
    default:
      break;
  }
}
  

//For controlling via WASD (any other key to stop)
void wasdMenu(){
  //Print out current actuator statuses
  Serial.print(" Azimuth: ");
  Serial.print(getAzi());
  Serial.print("Elevation: ");
  Serial.println(getEle());
  
  //Read in + execute command
  if (Serial.available()) {
    char command = (char) Serial.read();
    switch (command) {
      case 'w':
        haltAziEle();
        Serial.println("Going UP");
        incrEle();
        break;
      case 'a':
        haltAziEle();
        decrAzi();
        digitalWrite(LEFT_PIN,HIGH);
        break;
      case 's':
        haltAziEle();
        decrEle();
        digitalWrite(DOWN_PIN,HIGH);
        break;
      case 'd':
        haltAziEle();
        incrAzi();
        digitalWrite(RIGHT_PIN,HIGH);
        break;
      default:
        haltAziEle();
        break;
    }
  }
}


//For controlling via serial
void serialMenu(){
  double desiredAzi, desiredEle;
  commandIn(&desiredAzi, &desiredEle);
  setAzi(desiredAzi);
  setEle(desiredEle);
}


//Reads Azi/Ele from serial, stores in desiredAzi & desiredEle
// Commands in format: [<Azimuth angle * 1000>, <Elevation angle * 1000>]
void commandIn(double *desiredAzi, double *desiredEle){
  while(Serial.available() == 0){  // Waits for command
    delay(500);
  }
  long temp = 0;
  temp = Serial.parseInt();  //reads in an int, ignores all previous non-int characters
  *desiredAzi = temp / 1000.0;
  Serial.println(*desiredAzi, DEC);
  temp = Serial.parseInt();
  *desiredEle = temp / 1000.0;
  Serial.println(*desiredEle, DEC);
  temp = Serial.read();      //clears final ']'
}

