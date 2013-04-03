//states
#define DEFAULT_STATE  0
#define UP_STATE       1
#define NOTIFY_STATE   2
#define DOWN_STATE     3

int numRoutes = 4;
int currentRoute = 0;
float pointerPosition = 0; //percentage 0 till 1
int state = DEFAULT_STATE;
int motorSpeed = 0;
boolean motorDirFoward = true;

// pedal pins
int pedalInputTopPin
int pedalInputBottomPin
// route pins
int routeInputPins[] = {}; // input of route selection slider
int routeChoiceLedsPins[] = {} // led pins lightning route choice in closet
int routeLedsPins[] = { // led pins along routes to light objects
  {},
  {},
  {},
  {},
  {}
} 
// motor pins
int motorPosInputPin // motor position potmeter
int motorSpeedPin   =  // PWM / D2
int motorEnablePin  = 
int motorInversePin = 
int motorSFPin      = // status flag
int motorFBPin      = // analog power consumption feedback
// audio pins
int audioEnTXPin    = 
int audioEnRXPin    =
int audioNlTXPin    = 
int audioNlRXPin    = 

void setup() {
  // set pin modes
  
  //enable pull-up resistors
  
  Serial.begin(9600);
}

void loop()  { 
  
}
