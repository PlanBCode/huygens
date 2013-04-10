#define NUM_ROUTES 4
#define MAX_ROUTE_STEPS 6
#define NUM_OBJECTS 5
#define MOTOR_SPEED_DOWN 0.75
#define MOTOR_SPEED_UP 1
//states
#define DEFAULT_STATE  0
#define UP_STATE       1
#define NOTIFY_STATE   2
#define DOWN_STATE     3

#include "arduino.h"
#include "Pedal.h"
#include "AudioPlayer.h"
#include "PID.h"
#include "RouteSelector.h"
#include "LEDS.h"

enum objects { a, b, c, d, e };
int objectLedsPins[]  = { 
  52,50,48,46,44,42,40,38,36,34,32,30,53,51,49,47,45,43,41,39
}; // led pins along routes to light objects (pins 30 - 53)
float objectPositions[] = { 
  0.2,  0.3,  0.5,  0.4,  1
}; 
String objectTracks[]  = { 
  "1.MP3",  "2.MP3",  "3.MP3",  "4.MP3",  "5.MP3", "5.MP3"
}; // tracks per object along routes
int routes[] = {
  a, b, c, -1, -1, -1,
  a, c, e, -1, -1, -1,
  b, d, e, -1, -1, -1,
  b, d, e, -1, -1, -1,
};

float pointerTopPos      = 0.45; //0.47
float pointerBottomPos   = 0.17; //0.17; //0.13
float motorMaxFB         = 500; //(525mV per Amp) 525*5/5000*1024
int state                = DEFAULT_STATE;
int currentRoute         = 3;
int currentStep          = -1; // starts at -1 for not at first step yet
int currentObject        = -1;
int pedalPressedForce    = -1;

////////////// PINS //////////////
// motor pins
int motorPosInputPin     = A0; // motor position potmeter
int motorSpeedPin        = 12; // PWM / D2
//int motorSFPin         = 11; // status flag
int motorFBPin           = A1; // analog power consumption feedback
int motorEnablePin       = 10;
int motorSlewPin         = 9;
int motorInversePin      = 8;
// route pins
int routeInputPins[]     = {2,7,5,3}; // input of route selection slider
int routeChoiceLedsPins[] = {37,35,33,31}; // led pins lightning route choice in closet (pins 30 - 53)
// pedal pins
int pedalInputTopPin     = 21; //2;
int pedalInputBottomPin  = 20; //3;
int inputTopInterrupt    = 2; //0;
int inputBottomInterrupt = 3; //1;

////////////// PARTS //////////////
Pedal pedal(pedalInputTopPin,pedalInputBottomPin,
            inputTopInterrupt,inputBottomInterrupt,
            onPedalPressed);
RouteSelector routeSelector(routeInputPins, onRouteSelected);
PID pid(motorPosInputPin,motorSpeedPin,motorInversePin,motorEnablePin,motorFBPin);
Leds leds(objectLedsPins, routeChoiceLedsPins);
AudioPlayer audioPlayer(objectTracks, onAudioFinished);

void setup() {
  // set pin modes
  
  //enable pull-up resistors
  
  // move to TimePointer:
  digitalWrite(motorEnablePin,HIGH);
  digitalWrite(motorSlewPin,HIGH);
  pid.minPos = pointerBottomPos;
  pid.maxPos = pointerTopPos;
  pid.maxFB = motorMaxFB; 
  TCCR1B=(TCCR1B & 0xF8) | 0x01; //change PWM frequency (disabling noise)
  pid.gotoPos(0.5,MOTOR_SPEED_DOWN);
  
  Serial.begin(9600);
  Serial.println("");
  Serial.println("========================");
  Serial.println("");
  
  //pid.currentPos = 0.5;
  //state = UP_STATE;
  //audioPlayer.playObject(e);
}

void loop()  { 
  float pointerPos;
  
  
  while (Serial.available() > 0) {
    float value = Serial.parseFloat(); 
    Serial.println(value);
    
    pid.gotoPos(value,MOTOR_SPEED_UP);
    //pid.currentPos = value;
    state = UP_STATE;
  }  
  
  switch(state) {
    case DEFAULT_STATE: 
    
      routeSelector.update();
      pedal.update();
      
      
      break;
    case UP_STATE: 
      pid.update();
      
      //pid.currentPos = 0.5; // hardcode to half way
      pointerPos = float(round(pid.currentPos*10))/10;
      //pointerPos = pid.currentPos;
      //pointerPos = 0.40;
      Serial.print("pointerPos: ");
      Serial.println(pointerPos); 
      currentStep = -1;
      for(int i=0;i<MAX_ROUTE_STEPS;i++) {
        int object = routes[MAX_ROUTE_STEPS*currentRoute+i];
        Serial.print(" object: "); 
        Serial.print(object);
        if(object == -1) {
          Serial.println("");
          continue;
        }
        float stepPosition = objectPositions[object];
        Serial.print(" step: "); 
        Serial.print(i); 
        //Serial.print(" stepPosition: ");
        Serial.print(' ');
        Serial.println(stepPosition); 
        //if(pointerPos+0.01 < stepPosition) { //0.01 extra makes sure we also get past position1
          //leds.disableObject(object);
          //break;
        //} else {
        if(pointerPos >= stepPosition) { //0.01 extra makes sure we also get past position1
          leds.enableObject(object);
          currentStep = i;
          currentObject = object;
        }
      }
      
      Serial.print(" @ step: ");
      Serial.print(currentStep);
      Serial.print(" @ object: ");
      Serial.println(currentObject);
      
      //leds.enableObject(currentObject);
      
      Serial.print(pid.currentPos);
      Serial.print('/');
      Serial.print(pid.targetPos);
      Serial.print(' ');
      Serial.println(fabs(pid.targetPos-pid.currentPos));

      if(fabs(pid.targetPos-pid.currentPos) < 0.01) { // pointer at target pos?
        gotoState(NOTIFY_STATE);
      }
      break;
    case NOTIFY_STATE: 
    
      leds.disableAllObjects();
      leds.enableObject(currentObject);
      
      gotoState(DOWN_STATE);
      
      break;
    case DOWN_STATE: 

      pid.update();
      audioPlayer.update();
      break;
  }
  
  //delay(500);
}
void gotoState(int newState) {
  Serial.print("state > ");
  Serial.println(newState);
  float pointerPos;
  
  switch(newState) {
    case DEFAULT_STATE: 
      pedal.reset();
      leds.disableAllObjects();
      break;
    case UP_STATE: 

      // TODO choose random route ?  

      //float pointerPos = mapfloat(pedalPressedForce,0.0,1.0,pointerBottomPos,pointerTopPos);
      pointerPos = pedalPressedForce;
      pid.gotoPos(pointerPos,MOTOR_SPEED_UP);
      Serial.print(" > ");
      Serial.println(pointerPos);
  
      break;
    case NOTIFY_STATE: 

      break;
    case DOWN_STATE: 
      gotoStep(currentStep);
      break;
  }
  state = newState;
}
void onPedalPressed(float force) {
  if(state != DEFAULT_STATE) return;
  Serial.println("pedal pressed");
  Serial.print(force);
  
  
  pedalPressedForce = force;
  
  gotoState(UP_STATE);
}
void onRouteSelected(int route) {
  Serial.print("route: ");
  Serial.println(route);
  currentRoute = route;
  leds.lightRouteSelection(currentRoute);
}
void onAudioFinished() {
  Serial.println("audio finished");
  gotoStep(currentStep-1);
  
}
void gotoStep(int newStep) {
  Serial.print("next step: ");
  Serial.println(newStep);
  if(newStep >= 0) {
    currentObject = routes[MAX_ROUTE_STEPS*currentRoute+newStep];
    // highlight next object
    leds.enableObject(currentObject);
    // play next audio file
    audioPlayer.playObject(currentObject);
    // move pointer to object
    pid.gotoPos(objectPositions[currentObject],MOTOR_SPEED_DOWN);
  } else {
    gotoState(DEFAULT_STATE);
  }
  currentStep = newStep;
}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
