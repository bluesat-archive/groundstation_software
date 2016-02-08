//Mark Yeo; mark.yeo@student.unsw.edu.au
//Last Modified: 22Apr15
//Groundstation Code - Controller for Antenna Actuators
// Interfacing functions for actuators


#include <Arduino.h>
#include "actuator.h"


void initAzi(){
  pinMode(LEFT_PIN, OUTPUT);
  pinMode(RIGHT_PIN, OUTPUT);
  haltAzi();
}
void initEle(){
  pinMode(UP_PIN, OUTPUT);
  pinMode(DOWN_PIN, OUTPUT);
  haltEle();
}


//Returns the current azimuth/elevation actuator angle
double getAzi(void){
  double voltage = analogRead(AZI_PIN) * MAX_VOLTS / MAX_COUNTS;
  return (voltage - AZI_ZERO) * 180.0/(AZI_POS_90 - AZI_NEG_90); // deviation of voltage from zero * angle per volt
}
double getEle(void){
  double voltage = analogRead(ELE_PIN)*MAX_VOLTS/MAX_COUNTS;
  double degPerVolt = (ELE_MAX_ANGLE-0)/(ELE_MAX_VOLTS-ELE_MIN_VOLTS);
  double angleOffset = -ELE_MIN_VOLTS*degPerVolt;
  return degPerVolt * voltage + angleOffset; //check
}



//Controls actuators
void incrAzi(){
  digitalWrite(RIGHT_PIN,HIGH);
}
void decrAzi(){
  digitalWrite(LEFT_PIN,HIGH);
}
void haltAzi(){
  digitalWrite(LEFT_PIN, LOW);
  digitalWrite(RIGHT_PIN, LOW);
}
void setAzi(double desiredAzi){
  while (getAzi() < desiredAzi){
    incrAzi();
  }
  haltAzi();
  while (getAzi() > desiredAzi){
    decrAzi();
  }
  haltAzi();
}
void incrEle(){
  digitalWrite(UP_PIN,HIGH);
}
void decrEle(){
  digitalWrite(DOWN_PIN,HIGH);
}
void haltEle(){
  digitalWrite(UP_PIN, LOW);
  digitalWrite(DOWN_PIN, LOW);
}
void setEle(double desiredEle){
  while (getEle() < desiredEle){
    incrEle();
    //Serial.println(getEle());
  }
  haltEle();
  while (getEle() > desiredEle){
    decrEle();
    //Serial.println(getEle());
  }
  haltEle();
}
void haltAziEle(){
  haltAzi();
  haltEle();
}


