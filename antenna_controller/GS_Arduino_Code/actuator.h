//Mark Yeo; mark.yeo@student.unsw.edu.au
//Last Modified: 20Apr15
//Groundstation Code - Controller for Antenna Actuators
// Interfacing functions for actuators


//Pins
#define AZI_PIN 1
#define LEFT_PIN 10
#define RIGHT_PIN 11
#define AZI_NEG_90 1.028  //Voltage at Azi = -90deg
#define AZI_ZERO 2.3      //Voltage at Azi = 0deg
#define AZI_POS_90 3.573  //Voltage at Azi = -90deg

#define ELE_PIN 0
#define UP_PIN 9
#define DOWN_PIN 8
#define ELE_MIN_VOLTS 0.802   // Voltage at -180deg
#define ELE_MAX_VOLTS 4.997   // Voltage at +180deg //NOTE: May change when arduino is powered by computer
#define ELE_MAX_ANGLE 173     // Set via trial & error so that 90deg = actual 90deg

#define MAX_COUNTS 1023.0     // Max int returned by analogRead()
#define MAX_VOLTS 5.0         // Max voltage ready by analogRead()


void initAzi();
void initEle();

//Returns current azimuth/elevation actuator angle
double getAzi(void);
double getEle(void);

//Controls actuators
void incrAzi();
void decrAzi();
void haltAzi();
void setAzi(double desiredAzi);
void incrEle();
void decrEle();
void haltEle();
void setEle(double desiredEle);
void haltAziEle();
