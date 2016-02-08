/*
  BLUEsat Groundstation
  Arduino Serial to Actuators
  Definition of direciton input/output pins on the DB9 output cable made by Mark/Alexis
  Created: T Nguyen, 5-Oct-2014
  Last Modified: B Luc, 7-Dec-2015  
*/

#ifndef PINDEF_H
#define PINDEF_H

enum op_mode 
{
  DEBUG_MODE,  // Serial WASD control + current angle feedback
  MANUAL_MODE, // Set actuator angles through EL & AZ constants
  AUTO_MODE,    // Reads desired angles in from Serial; Format: "[Ele,Azi]"; where Ele & Azi are desired degrees * 1000
  QB50_MODE    // Cameron's QB50 Implementation
};

#define DOWN_PIN 8
#define UP_PIN 9
#define LEFT_PIN 10
#define RIGHT_PIN 11
//#define ELEVATION_PIN 0
//#define AZIMUTH_PIN 1
#define ELEVATION_PIN 1
#define AZIMUTH_PIN 0
#define QB50_MODE_PIN 12


#define ON 1
#define OFF 0

#endif
