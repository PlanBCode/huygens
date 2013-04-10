#define NUM_ROUTES 4
#define MAX_ROUTE_STEPS 6
#define NUM_OBJECTS 5
#define MOTOR_SPEED_DOWN 0.75
#define MOTOR_SPEED_UP 1
#define NUM_LEDS 20
#define MAX_ENABLED_LEDS 5      // limit number of simultaneous LEDs on to avoid powersupply overload

//states
#define DEFAULT_STATE  0
#define UP_STATE       1
#define NOTIFY_STATE   2
#define DOWN_STATE     3

#define DEBUG
#define DEBUG_FLOW
//#define DEBUG_SERIAL
//#define DEBUG_POSITION_OBJECT_MAPPING
//#define DEBUG_AUDIO_PLAYER_SERIAL
#define DEBUG_LEDS 
#define DEBUG_PID
//#define DEBUG_PID_VERBOSE
//#define DEBUG_PEDAL
//#define DEBUG_PEDAL_VERBOSE
//#define DEBUG_ROUTE_SELECTOR
//#define DISABLE_LEDS

#include "arduino.h"
#include "Pedal.h"
#include "AudioPlayer.h"
#include "PID.h"
#include "RouteSelector.h"
#include "LEDS.h"

enum objects { a, b, c, d, e };
// led pins along routes to light objects (pins 30 - 53) 
// add ALL leds pins to make sure they are pulled down when off!
int objectLedsPins[]  = { 
  //52,50,48,46,44,42,40,38,36,34,32,30,53,51,49,47,45,43,41,39
  38,36,34,32,30,53,51,49,47,45,43,41,39,  52,50,48,46,44,42,40,
}; 
float objectPositions[] = { 
  0.2,  0.3,  0.5,  0.75,  1
}; 
String objectTracks[]  = { 
  "1.MP3",  "2.MP3",  "3.MP3",  "4.MP3",  "5.MP3", "5.MP3"
}; // tracks per object along routes
int routes[] = {
  a, b, c,  d,  e, -1,
  a, c, e, -1, -1, -1,
  b, c, d, -1, -1, -1,
  b, d, e, -1, -1, -1,
  c, d, e,  a, -1, -1,
  d, e, a, -1, -1, -1
};
int routeStartObjects[] = {a,b,c,d}; // objects in closed that people select

float pointerTopPos      = 0.45; //0.47 // (unmapped) //TODO change to raw int
float pointerBottomPos   = 0.17; //0.17; //0.13 // (unmapped) //TODO change to raw int
float motorMaxFB         = 500; //(525mV per Amp) 525*5/5000*1024
int state                = DEFAULT_STATE;
int currentRoute         = 0;
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
  
  // motor
  digitalWrite(motorEnablePin,HIGH);
  digitalWrite(motorSlewPin,HIGH);
  pid.minPos = pointerBottomPos;
  pid.maxPos = pointerTopPos;
  pid.maxFB = motorMaxFB; 
  TCCR1B=(TCCR1B & 0xF8) | 0x01; //change PWM frequency (disabling noise)
  pid.gotoPos(0.5,MOTOR_SPEED_DOWN);
  
  #ifdef DEBUG 
    Serial.begin(9600);
    Serial.println("");
    Serial.println("========================");
    Serial.println("");
  #endif
  
  //pid.currentPos = 0.5;
  //state = UP_STATE;
  //audioPlayer.playObject(a);
  
  gotoRoute(currentRoute);
  
  /*leds.enableObject(a);
  leds.enableObject(c);
  leds.enableObject(e);
  leds.disableObject(c);
  leds.disableObject(b);
  leds.disableAllObjects();
  leds.enableObject(c);*/
}

void loop()  { 
  float pointerPos;
  
  #ifdef DEBUG_SERIAL
    while (Serial.available() > 0) {
      float value = Serial.parseFloat(); 
      Serial.println(value);
      
      pid.gotoPos(value,MOTOR_SPEED_UP);
      //pid.currentPos = value;
      state = UP_STATE;
    }  
  #endif
  
  //Serial.println(state);
  
  switch(state) {
    case DEFAULT_STATE: 
    
      routeSelector.update();
      pedal.update();
      
      
      break;
    case UP_STATE: 
      
      //pid.currentPos = 0.5; // hardcode to half way
      pointerPos = float(round(pid.currentPos*10))/10; // rounding to make sure it reaches 1
      //pointerPos = pid.currentPos;
      //pointerPos = 0.40;
      #ifdef DEBUG_POSITION_OBJECT_MAPPING
        Serial.print("pointerPos: ");
        Serial.println(pointerPos); 
        Serial.print(" route: ");
        Serial.println(currentRoute); 
      #endif
      currentStep = -1;
      for(int i=0;i<MAX_ROUTE_STEPS;i++) {
        int object = routes[MAX_ROUTE_STEPS*currentRoute+i];
        
        #ifdef DEBUG_POSITION_OBJECT_MAPPING
          Serial.print(" object: "); 
          Serial.print(object);
        #endif
        if(object == -1) {
          #ifdef DEBUG_POSITION_OBJECT_MAPPING
            Serial.println("");
          #endif
          continue;
        }
        float objectPosition = objectPositions[object];
        #ifdef DEBUG_POSITION_OBJECT_MAPPING
          Serial.print(" step: "); 
          Serial.print(i); 
          Serial.print(" stepPosition: ");
          Serial.print(' ');
          Serial.println(stepPosition); 
        #endif
        //if(pointerPos < stepPosition) {
          //leds.disableObject(object);
          //break;
        //} else {
        if(pointerPos >= objectPosition) { 
          leds.enableObject(object);
          currentStep = i;
          currentObject = object;
        } else {
          leds.disableObject(object);
        }
      }
      #ifdef DEBUG_POSITION_OBJECT_MAPPING
        Serial.print(" @ step: ");
        Serial.print(currentStep);
        Serial.print(" @ object: ");
        Serial.println(currentObject);
      #endif
      //leds.enableObject(currentObject);
      
      #ifdef DEBUG_FLOW
        //Serial.print(pid.currentPos);
        //Serial.print('/');
        //Serial.println(pid.targetPos);
      #endif
      
      if(fabs(pid.targetPos-pid.currentPos) < 0.01) { // pointer at target pos?
        gotoState(NOTIFY_STATE);
      }
      break;
    case NOTIFY_STATE: 
      
      break;
    case DOWN_STATE: 
      
      #ifdef DEBUG_FLOW
        //if(millis())
        //Serial.print(pid.currentPos);
        //Serial.print('/');
        //Serial.println(pid.targetPos);
      #endif
      
      if(fabs(0-pid.currentPos) < 0.01) { // pointer at bottom?
        gotoState(DEFAULT_STATE);
      }
      
      break;
  }
  pid.update();
  audioPlayer.update();
  
  //delay(500);
}

void gotoState(int newState) {
  #ifdef DEBUG_FLOW
    Serial.print("state > ");
    Serial.println(newState);
  #endif
  state = newState;
  float pointerTargetPos;
  
  switch(newState) {
    case DEFAULT_STATE: 
      pedal.reset(); // reset state of pedal (could have changed by interrupt
      leds.disableAllObjects();
      
      break;
    case UP_STATE: 
      
      // TODO choose random route ?  
      //routeStartObjects
      
      //float pointerPos = mapfloat(pedalPressedForce,0.0,1.0,pointerBottomPos,pointerTopPos);
      pointerTargetPos = pedalPressedForce;
      pid.gotoPos(pointerTargetPos,MOTOR_SPEED_UP);
      #ifdef DEBUG_FLOW
        Serial.print(" > ");
        Serial.println(pointerTargetPos);
      #endif
      break;
    case NOTIFY_STATE: 
      Serial.println("@ notify state");
      delay(1000);
      
      leds.disableAllObjects();
      //Serial.print("currentObject: ");
      //Serial.println(currentObject);
      leds.enableObject(currentObject);
      
      delay(1000);
      Serial.println("called DOWN_STATE");
      gotoState(DOWN_STATE);
      
      break;
    case DOWN_STATE: 
      Serial.println("@ down state");
      gotoStep(currentStep);
      break;
  }
  
  Serial.print("@ state: ");
  Serial.println(state);
}

void onPedalPressed(float force) {
  if(state != DEFAULT_STATE) return;
  #ifdef DEBUG_FLOW
    Serial.print("pedal pressed: ");
    Serial.println(force);
  #endif
  
  pedalPressedForce = force;
  
  gotoState(UP_STATE);
}

void onRouteSelected(int route) {
  gotoRoute(route);
}

void onAudioFinished() {
  #ifdef DEBUG_FLOW
    Serial.println("audio finished");
  #endif
  //delay(1000); //TEMP
  gotoStep(currentStep-1); 
}

void gotoRoute(int route) {
  #ifdef DEBUG_FLOW
    Serial.print("goto route: ");
    Serial.println(route);
  #endif
  currentRoute = route;
  leds.lightRouteSelection(currentRoute);
}

void gotoStep(int newStep) {
  #ifdef DEBUG_FLOW
    Serial.print("goto step: ");
    Serial.println(newStep);
  #endif
  if(newStep >= 0) { // is stepping to an object
    currentObject = routes[MAX_ROUTE_STEPS*currentRoute+newStep];
    // highlight next object
    leds.enableObject(currentObject);
    // play next audio file
    audioPlayer.playObject(currentObject);
    // move pointer to object
    pid.gotoPos(objectPositions[currentObject],MOTOR_SPEED_DOWN);
  } else { // if done route
    pid.gotoPos(0,MOTOR_SPEED_DOWN);
    //gotoState(DEFAULT_STATE);
  }
  currentStep = newStep;
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
