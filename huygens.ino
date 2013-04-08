#include "arduino.h"
#include "Pedal.h"
//#include "TimePointer.h"
#include "PID.h"
#include "RouteSelector.h"

//states
#define DEFAULT_STATE  0
#define UP_STATE       1
#define NOTIFY_STATE   2
#define DOWN_STATE     3

int numRoutes             = 4;
int currentRoute          = 0;
int routeTracks[5][1]     = { 
  {},
  {},
  {},
  {},
  {}
}; // tracks per object along routes
float pointerEndPosition = 0; //percentage 0 till 1
int state                = DEFAULT_STATE;
float motorSpeed         = 1;
boolean motorDirFoward   = true;

// motor pins
int motorPosInputPin     = A0; // motor position potmeter
int motorSpeedPin        = 12; // PWM / D2
//int motorSFPin         = 11; // status flag
int motorFBPin           = A1; // analog power consumption feedback
int motorEnablePin       = 10;
int motorSlewPin         = 9;
int motorInversePin      = 8;
// route pins
int routeInputPins[]      = {2,7,5,3}; // input of route selection slider
int routeChoiceLedsPins[] = {}; // led pins lightning route choice in closet (pins 30 - 53)
int routeLedsPins[5][1]       = { 
  {},
  {},
  {},
  {},
  {}
}; // led pins along routes to light objects (pins 30 - 53)
// pedal pins
int pedalInputTopPin    = 21; //2;
int pedalInputBottomPin = 20; //3;
int inputTopInterrupt   = 2; //0;
int inputBottomInterrupt= 3; //1;

// audio pins
//int audioTXPin      = 18;
//int audioRXPin      = 19;

Pedal pedal(pedalInputTopPin,pedalInputBottomPin,
            inputTopInterrupt,inputBottomInterrupt,
            onPedalPressed);

RouteSelector routeSelector(routeInputPins, routeChoiceLedsPins, numRoutes);

PID pid(motorPosInputPin,motorSpeedPin,motorInversePin,motorEnablePin,motorFBPin);

void setup() {
  // set pin modes
  
  //enable pull-up resistors
  
  // move to TimePointer:
  digitalWrite(motorEnablePin,HIGH);
  digitalWrite(motorSlewPin,HIGH);
  //setPwmFrequency(motorSpeedPin,1);
  TCCR1B=(TCCR1B & 0xF8) | 0x01;
  pid.gotoPos(0.31,motorSpeed);
  
  
  Serial.begin(9600);
}

void loop()  { 
  
  pedal.update();
  
  routeSelector.update();
  
  while (Serial.available() > 0) {
    float value = Serial.parseFloat(); 
    Serial.println(value);
    
    pid.gotoPos(value,motorSpeed);
    
    // half: 0.3
    /*float topPos = 0.13;
    float bottomPos = 0.47;
    float force = 0;
    float pointerPos = 0;
    
    force = Serial.parseFloat(); 
    pointerPos = mapfloat(force,0.0,1.0,bottomPos,topPos);
    Serial.println(pointerPos);
    
    pid.gotoPos(pointerPos,motorSpeed);*/
  }
  pid.update();
  
  //delay(10);
}
void onPedalPressed(float force) {
  Serial.print(force);
  
  float topPos = 0.45; //0.47
  float bottomPos = 0.17; //0.17; //0.13
  float pointerPos = 0;
  
  pointerPos = mapfloat(force,0.0,1.0,bottomPos,topPos);
  pid.gotoPos(pointerPos,motorSpeed);
  
  
  Serial.print(" > ");
  Serial.println(pointerPos);
 
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
