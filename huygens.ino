#include <PID_v1.h>
#include "Arduino.h"
#include "content.h"

#define MOTOR_SPEED_DOWN 0.50
#define MOTOR_SPEED_UP 1
#define NUM_LEDS 25
#define MAX_ENABLED_LEDS 7      // limit number of simultaneous LEDs on to avoid powersupply overload

//states
#define DEFAULT_STATE  0
#define UP_STATE       1
#define NOTIFY_STATE   2
#define DOWN_STATE     3

//#define DEBUG
//#define DEBUG_FLOW
//#define DEBUG_SERIAL
//#define DEBUG_ALWAYS_TO_TOP 
//#define DEBUG_CONTENT
//#define DEBUG_POSITION_OBJECT_MAPPING
//#define DEBUG_ROUTE_SELECTION
//#define DEBUG_AUDIO_PLAYER
//#define DEBUG_AUDIO_PLAYER_SERIAL
//#define DEBUG_LEDS 
//#define DEBUG_PID
//#define DEBUG_PID_VERBOSE
//#define DEBUG_PEDAL
//#define DEBUG_PEDAL_VERBOSE
//#define DEBUG_START_OBJECT_SELECTOR
//#define DISABLE_LEDS
//#define OVERRULE_ROUTE 2 // overrule selected route 
//#define OVERRULE_TARGET_POS 0.90 // overrule the position of the TijdWijzer when the pedal is pressed
//#define OVERRULE_AUDIO_DURATION 5000 // limit audio duration (time per route step) for quick testing

//#include "TimerThree.h"
#include "Pedal.h"
#include "AudioPlayer.h"
#include "StartObjectSelector.h"
#include "Leds.h"
#include "TijdWijzer.h"

unsigned int count;

float pointerTopPos      = 423; //475; //0.43; //0.45; //0.47 // (unmapped)
float pointerBottomPos   = 67; //137; //0.15; //0.17; //0.17; //0.13 // (unmapped)
int state                = DEFAULT_STATE;
int currentRoute         = 0;
int currentStep          = -1; // starts at -1 for not at first step yet
int currentObject        = -1;
int pedalPressedForce    = -1;
float pointerTargetPos   = 0;

// position debug for DEBUG_FLOW
long unsigned int prevPosPrintTime     = 0;
int posPrintInterval     = 1000;
float prevPrintedPos     = 0;

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
//int startObjectInputPins[] = {2,7,5,3}; // input of start object selection slider
int startObjectInputPins[] = {1,6,4,2}; // input of start object selection slider

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
Tijdwijzer tijdWijzer(motorPosInputPin, motorSpeedPin, motorInversePin, motorEnablePin, motorSlewPin, motorFBPin, pointerBottomPos, pointerTopPos);
Leds leds(objectLedsPins);
AudioPlayer audioPlayer(objectTracks, onAudioFinished);

void setup() {
  count = 0;
  
  // motor
  tijdWijzer.gotoPos(0.0,MOTOR_SPEED_DOWN);
  
//  pinMode(3, OUTPUT);
//  Timer3.initialize(10000); // initialize timer, and set a 10000 millisecond period
//  Timer3.pwm(3, 512); // setup pwm on pin 3 (nterrupt 1), 50% duty cycle
//  Timer3.attachInterrupt(booboo);  // attaches callback() as a timer overflow interrupt
        
  #ifdef DEBUG 
    Serial.begin(9600);
    Serial.println("");
    Serial.println("========================");
    Serial.println("");
  #endif
  
  //pid.currentPos = 0.5;
  //state = UP_STATE;
  //audioPlayer.playObject(a);
  
  #ifdef DEBUG_CONTENT
    for(int i = 0;i<NUM_OBJECTS;i++) {
      Serial.print(i);
      Serial.print('\t');
      Serial.print(objectNames[i]);
      Serial.print('\t');
      Serial.print(objectPositions[i]);
      Serial.print('\t');
      Serial.print(objectLedsPins[i]);
      Serial.print('\t');
      Serial.println(objectTracks[i]);
    }
    Serial.println("");
    for(int i = 0;i<NUM_START_OBJECTS;i++) {
      Serial.print(routeStartObjects[i]);
      Serial.print('\t');
      Serial.print(objectNames(routeStartObjects[i]));
      Serial.print('\t');
      Serial.println(objectLedsPins[routeStartObjects[i]]);
    }
    Serial.println("");
  #endif
  setStartObject(currentStartObject);
  
  /*leds.enableObject(a);
  leds.enableObject(c);
  leds.enableObject(e);
  leds.disableObject(c);
  leds.disableObject(b);
  leds.disableAllObjects();
  leds.enableObject(c);*/
}

//void booboo() {
  //pid.update();
//  tijdWijzer.update();
//}

void loop()  { 
  float pointerPos;
/*  if (millis()%1000 == 0) {
    Serial.print(count/1000);
    Serial.println("cycles/ms");
    count = 0;
  }
  else count++;
  */
  tijdWijzer.update();
  
  #ifdef DEBUG_SERIAL
    while (Serial.available() > 0) {
      float value = Serial.parseFloat(); 
      Serial.println(value);
      
      //pid.gotoPos(value,MOTOR_SPEED_UP);
      tijdWijzer.gotoPos(value,MOTOR_SPEED_UP);
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
      //pointerPos = float(round(pid.currentPos*10))/10; // rounding to make sure it reaches 1
      pointerPos = float(round(tijdWijzer.getPosition()*10))/10; // rounding to make sure it reaches 1
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
        //if(round(millis()/20)*20%1000 == 0) {
        if(millis()-prevPosPrintTime > posPrintInterval && prevPrintedPos != tijdWijzer.getPosition()) {
          Serial.print(tijdWijzer.getPosition());
          Serial.print('/');
          Serial.println(tijdWijzer.getTargetPosition());
          prevPosPrintTime = millis();
          prevPrintedPos = tijdWijzer.getPosition();
        }
      #endif
      
      if(fabs(tijdWijzer.getTargetPosition()-tijdWijzer.getPosition()) < 0.01) { // pointer at target pos?
        gotoState(NOTIFY_STATE);
      }
      break;
    case NOTIFY_STATE: 
      
      break;
    case DOWN_STATE: 
      
      #ifdef DEBUG_FLOW
        //if(millis())
        if(millis()-prevPosPrintTime > posPrintInterval && prevPrintedPos != tijdWijzer.getPosition()) {
          Serial.print(tijdWijzer.getPosition());
          Serial.print('/');
          Serial.println(tijdWijzer.getTargetPosition());
          prevPosPrintTime = millis();
          prevPrintedPos = tijdWijzer.getPosition();
        }
      #endif
      
      /*if(fabs(0-tijdWijzer.getPosition()) < 0.01) { // pointer at bottom?
        gotoState(DEFAULT_STATE);
      }*/
      
      break;
  }
//  pid.update();
  audioPlayer.update();
  
  //delay(500);
}

void gotoState(int newState) {
  #ifdef DEBUG_FLOW
    Serial.print("state > ");
    Serial.println(newState);
  #endif
  state = newState;
  
  switch(newState) {
    case DEFAULT_STATE: 
      pedal.reset(); // reset state of pedal (could have changed by interrupt
      leds.disableAllObjects();
      
      setStartObject(currentStartObject);
      
      break;
    case UP_STATE: 

      pickRoute();
      
      #ifdef DEBUG_FLOW
        Serial.print("selected route: "); 
        Serial.println(currentRoute);
      #endif
      
      //routeStartObjects
      
//      float pointerPos = mapfloat(pedalPressedForce,0.0,1.0,pointerBottomPos,pointerTopPos);
      //pointerTargetPos = min(0.25 + 0.83*pedalPressedForce, 1.0);
      
//      pointerTargetPos = pedalPressedForce;
      //pid.gotoPos(pointerTargetPos,MOTOR_SPEED_UP);
      tijdWijzer.gotoPos(pointerTargetPos,MOTOR_SPEED_UP);
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
    Serial.print(force);
  #endif
  
  pedalPressedForce = force;
  
  pointerTargetPos = min(0.35 + 0.75*force, 1.0);
  
  #ifdef OVERRULE_TARGET_POS
    pointerTargetPos = OVERRULE_TARGET_POS;
  #endif
  
  #ifdef DEBUG_ALWAYS_TO_TOP
    pointerTargetPos = 1;
  #endif
  
  #ifdef DEBUG_FLOW
    Serial.print(" > ");
    Serial.println(pointerTargetPos);
  #endif
  
  #ifndef DEBUG_PEDAL
    gotoState(UP_STATE);
  #endif
}

void onStartObjectSelected(int startObjectPos) {
  int startObject = routeStartObjects[startObjectPos];
  setStartObject(startObject);
}

void onAudioFinished() {
  #ifdef DEBUG_FLOW
    Serial.println("audio finished");
  #endif
  //delay(1000); //TEMP
  gotoStep(currentStep-1); 
}

void setStartObject(int newStartObject) {
  #ifdef DEBUG_FLOW
    Serial.println("");
    Serial.print("new startobject: ");
    Serial.print(newStartObject);
    Serial.print(' ');
    Serial.println(objectNames[newStartObject]);
  #endif
  currentStartObject = newStartObject;
  //currentRoute = route;
  //leds.lightStartObjectSelection(newStartObject);
  
  for(int i=0;i<NUM_START_OBJECTS;i++) {
    int startObject = routeStartObjects[i];
    if(startObject == newStartObject) {
      leds.enableObject(startObject);
    } else {
      leds.disableObject(startObject);
    }
  }
  
  #ifdef DEBUG_ROUTE_SELECTION
    pickRoute();
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
    //int routeStartObject = routes[MAX_ROUTE_STEPS*i];
    int routeStartObject = startObjectsPerRoute[i];
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
    //int routeStartObject = routes[MAX_ROUTE_STEPS*i];
    int routeStartObject = startObjectsPerRoute[i];
    if(routeStartObject == currentStartObject) {
      if(compatibleRouteIndex == randomRouteIndex) {
        currentRoute = i;   
      }
      compatibleRouteIndex++;
    }
  }
  
  #ifdef OVERRULE_ROUTE
    currentRoute = OVERRULE_ROUTE;
  #endif
  
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
    
    Serial.print("  currentObject: ");
    Serial.print(currentObject);
    Serial.print(' ');
    Serial.println(objectNames[currentObject]);
    
    // highlight next object
    leds.enableObject(currentObject);
    // play next audio file
    audioPlayer.playObject(currentObject);
    // move pointer to object
    //pid.gotoPos(objectPositions[currentObject],MOTOR_SPEED_DOWN);
    tijdWijzer.gotoPos(objectPositions[currentObject],MOTOR_SPEED_DOWN);
    
  } else { // if done route
    //pid.gotoPos(0,MOTOR_SPEED_DOWN);
    tijdWijzer.gotoPos(0,MOTOR_SPEED_DOWN);
    gotoState(DEFAULT_STATE);
  }
  currentStep = newStep;
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
