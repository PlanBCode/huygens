//states
#define DEFAULT_STATE  0
#define UP_STATE       1
#define NOTIFY_STATE   2
#define DOWN_STATE     3

int numRoutes = 4;
int currentRoute = 0;
int routeTracks[] = { // tracks per object along routes
  {},
  {},
  {},
  {},
  {}
} 
float pointerEndPosition = 0; //percentage 0 till 1
int state                = DEFAULT_STATE;
int motorSpeed           = 0;
boolean motorDirFoward   = true;

// motor pins
int motorPosInputPin  = A0; // motor position potmeter
int motorSpeedPin     = 12; // PWM / D2
int motorSFPin        = 11; // status flag
//int motorFBPin      = A0; // analog power consumption feedback
int motorEnablePin    = 10;
int motorSlewPin      = 9;
int motorInversePin   = 8;
// route pins
int routeInputPins[]      = {7,6,5,4}; // input of route selection slider
int routeChoiceLedsPins[] = {} // led pins lightning route choice in closet (pins 30 - 53)
int routeLedsPins[]       = { // led pins along routes to light objects (pins 30 - 53)
  {},
  {},
  {},
  {},
  {}
} 
// pedal pins
int pedalInputTopPin    = 3
int pedalInputBottomPin = 2
// audio pins
//int audioTXPin      = 18;
//int audioRXPin      = 19;

void setup() {
  // set pin modes
  
  //enable pull-up resistors
  
  digitalWrite(motorSlewPin,LOW);
  
  Serial.begin(9600);
}

void loop()  { 
  
}
