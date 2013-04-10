#define NUM_START_OBJECTS 4
#define NUM_ROUTES 6
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
//#define DEBUG_ROUTE_SELECTION
//#define DEBUG_AUDIO_PLAYER_SERIAL
#define DEBUG_LEDS 
//#define DEBUG_PID
//#define DEBUG_PID_VERBOSE
//#define DEBUG_PEDAL
//#define DEBUG_PEDAL_VERBOSE
//#define DEBUG_START_OBJECT_SELECTOR
//#define DISABLE_LEDS

#include "Arduino.h"
#include "Pedal.h"
#include "AudioPlayer.h"
#include "PID.h"
#include "StartObjectSelector.h"
#include "Leds.h"

enum objects { a, b, c, d, e };
// led pins along routes to light objects (pins 30 - 53) 
// add ALL leds pins to make sure they are pulled down when off!
int objectLedsPins[]  = { 
  52,50,48,46,44,42,40,38,36,34,32,30,53,51,49,47,45,43,41,39
  //38,36,34,32,30,53,51,49,47,45,43,41,39,  52,50,48,46,44,42,40,
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

float pointerTopPos      = 475; //0.43; //0.45; //0.47 // (unmapped)
float pointerBottomPos   = 137; //0.15; //0.17; //0.17; //0.13 // (unmapped)
float motorMaxFB         = 500; //(525mV per Amp) 525*5/5000*1024
int state                = DEFAULT_STATE;
int currentStartObject   = a;
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
// startObjects pins
int startObjectInputPins[] = {2,7,5,3}; // input of start object selection slider
int startObjectLedsPins[]  = {37,35,33,31}; // led pins lightning start object choice in closet (pins 30 - 53)
// pedal pins
int pedalInputTopPin     = 21; //2;
int pedalInputBottomPin  = 20; //3;
int inputTopInterrupt    = 2; //0;
int inputBottomInterrupt = 3; //1;

////////////// PARTS //////////////
Pedal pedal(pedalInputTopPin,pedalInputBottomPin,
            inputTopInterrupt,inputBottomInterrupt,
            onPedalPressed);
StartObjectSelector startObjectSelector(startObjectInputPins, onStartObjectSelected);
PID pid(motorPosInputPin,motorSpeedPin,motorInversePin,motorEnablePin,motorFBPin);
Leds leds(objectLedsPins, startObjectLedsPins);
AudioPlayer audioPlayer(objectTracks, onAudioFinished);

void setup() {
  
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
  
  setStartObject(a);
  
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
      //state = UP_STATE;
    }  
  #endif
  
  //Serial.println(state);
  
  switch(state) {
    case DEFAULT_STATE: 
    
      startObjectSelector.update();
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
      }
      
      #ifdef DEBUG_FLOW
        if(round(millis()/100)*100%500 == 0) {
          Serial.print(pid.currentPos);
          Serial.print('/');
          Serial.println(pid.targetPos);
        }
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
        if(round(millis()/100)*100%500 == 0) {
          Serial.print(pid.currentPos);
          Serial.print('/');
          Serial.println(0);
        }
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

      pickRoute();
      
      #ifdef DEBUG_FLOW
        Serial.print("selected route: "); 
        Serial.println(currentRoute);
      #endif
      
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
      delay(1000);
      
      leds.disableAllObjects();
      //Serial.print("currentObject: ");
      //Serial.println(currentObject);
      leds.enableObject(currentObject);
      
      delay(1000);
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

void onStartObjectSelected(int startObject) {
  setStartObject(startObject);
}

void onAudioFinished() {
  #ifdef DEBUG_FLOW
    Serial.println("audio finished");
  #endif
  //delay(1000); //TEMP
  gotoStep(currentStep-1); 
}

void setStartObject(int startObject) {
  #ifdef DEBUG_FLOW
    Serial.print("new startobject: ");
    Serial.println(startObject);
  #endif
  currentStartObject = startObject;
  //currentRoute = route;
  leds.lightStartObjectSelection(startObject);
  
  
  #ifdef DEBUG_ROUTE_SELECTION
    currentRoute = pickRoute();
  #endif
}
void pickRoute() {
  #ifdef DEBUG_ROUTE_SELECTION
    Serial.println("pickRoute");
    Serial.print("currentStartObject: ");
    Serial.println(currentStartObject);
  #endif
  int numCompatibleRoutes = 0; // count routes that start with selected startObject
  for(int i=0;i<NUM_ROUTES;i++) {
    int routeStartObject = routes[MAX_ROUTE_STEPS*i];
    #ifdef DEBUG_ROUTE_SELECTION
      Serial.print(' ');
      Serial.print(i);
      Serial.print(' ');
      Serial.print(routeStartObject);
    #endif
    if(routeStartObject == currentStartObject) {
      numCompatibleRoutes++;
      #ifdef DEBUG_ROUTE_SELECTION
        Serial.print(" compatible route");  
      #endif
    }
    #ifdef DEBUG_ROUTE_SELECTION
      Serial.println(' ');
    #endif
  }
  int randomRouteIndex = random(0,numCompatibleRoutes);
  #ifdef DEBUG_ROUTE_SELECTION
    Serial.print("numCompatibleRoutes: ");
    Serial.println(numCompatibleRoutes);
    Serial.print("randomRouteIndex: ");
    Serial.println(randomRouteIndex);
  #endif
  int compatibleRouteIndex = 0; 
  for(int i=0;i<NUM_ROUTES;i++) {
    int routeStartObject = routes[MAX_ROUTE_STEPS*i];
    if(routeStartObject == currentStartObject) {
      if(compatibleRouteIndex == randomRouteIndex) {
        currentRoute = i;   
      }
      compatibleRouteIndex++;
    }
  }
  #ifdef DEBUG_ROUTE_SELECTION
    Serial.print("picked route: "); 
    Serial.println(currentRoute);
  #endif
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
