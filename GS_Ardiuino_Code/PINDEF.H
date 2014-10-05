/*
  BLUEsat Groundstation
  Arduino Code
  Definition of direciton input/output pins on the DB9 output cable made by Mark/Alexis
  Created: T Nguyen, 5-Oct-2014
  Last Modified: T Nguyen, 5-Oct-2014  
*/

#ifndef PINDEF_H
#define PINDEF_H

enum op_mode 
{
  DEBUG_MODE,
  NORMAL_OPERATION
};

#define DOWN_PIN 8
#define UP_PIN 9
#define LEFT_PIN 10
#define RIGHT_PIN 11
#define ELEVATION_PIN 0
#define AZIMUTH_PIN 1

#define ON 1
#define OFF 0

#endif
