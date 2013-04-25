#define PEDAL_DEBOUNCE_TIME 10
#define PEDAL_MAX_PRESSING_TIME 700

// pins
int __pedalInputTopPin;
int __pedalInputBottomPin;

volatile bool __pedalPressing;
volatile bool __pedalPressed;

volatile long unsigned int __pedalStartPressingTime;
volatile long unsigned int __pedalEndPressingTime;
volatile long unsigned int __pedalPressingTime;

// debounce
volatile long unsigned int __pedalTopChangedTime;
volatile boolean __pedalTopPrevState;

void pedalTopChanged() {
  
  if(millis()-__pedalTopChangedTime > PEDAL_DEBOUNCE_TIME) {// debounce prevention
    //Serial.println("tx");
    boolean pedalTopState = digitalRead(__pedalInputTopPin);
    if(pedalTopState != __pedalTopPrevState) {
      if(pedalTopState == HIGH) { // if left top (for actual pedal change to HIGH
        //Serial.println('t');
        __pedalStartPressingTime = millis();
        __pedalPressing = true;
      } else if(__pedalPressing && !__pedalPressed){ //if(__pedalPressing && !__pedalPressed){ // if it reaches the top again and the pedal wasn't fully pressed down
        //Serial.println("t2");
        __pedalPressingTime = PEDAL_MAX_PRESSING_TIME*0.8;
        __pedalPressing = false;
        __pedalPressed = true;
      }
      __pedalTopPrevState = pedalTopState;
      __pedalTopChangedTime = millis();
    } 
  }
}
void pedalBottomChanged() {
  //Serial.print('b');
  if(__pedalPressing && digitalRead(__pedalInputBottomPin) == LOW) { // if reached bottom
    //Serial.println('b');
    __pedalEndPressingTime = millis();
    __pedalPressingTime = __pedalEndPressingTime-__pedalStartPressingTime;
    __pedalPressing = false;
    __pedalPressed = true;
  }
}

class Pedal {
  
  private: 
    typedef void (*PedalPressedHandler)(float force);
    PedalPressedHandler pressedHandler;
  
  public: 
    //int maxPressingTime;
    int pressingTime;
    float force;
    
    Pedal(int inputTopPin, int inputBottomPin, 
          int inputTopInterrupt, int inputBottomInterrupt, 
          PedalPressedHandler _pressedHandler) {
      __pedalInputTopPin = inputTopPin;
      __pedalInputBottomPin = inputBottomPin;
      pressedHandler = _pressedHandler;
      
      __pedalPressing = false;
      __pedalPressed = false;
      __pedalStartPressingTime = 0;
      __pedalPressingTime = 0;
      
      __pedalTopChangedTime = 0;
      __pedalTopPrevState = LOW;
      
      pressingTime = 0;
      force = 0;
      
      pinMode(inputTopPin,INPUT);
      pinMode(inputBottomPin,INPUT);
      
      // enable pull-up resistors
      digitalWrite(inputTopPin,HIGH);
      digitalWrite(inputBottomPin,HIGH);
      
      attachInterrupt(inputTopInterrupt, pedalTopChanged, CHANGE);
      attachInterrupt(inputBottomInterrupt, pedalBottomChanged, CHANGE);
    }
    void reset() {
      __pedalPressed = false;
    }
    void update() {
      #ifdef DEBUG_PEDAL_VERBOSE
        Serial.print(digitalRead(__pedalInputTopPin));
        Serial.print('\t');
        Serial.println(digitalRead(__pedalInputBottomPin));
      #endif
      
      if(__pedalPressed && __pedalPressingTime <= PEDAL_MAX_PRESSING_TIME) {
        pressingTime = __pedalPressingTime;
        force = float(PEDAL_MAX_PRESSING_TIME-pressingTime)/PEDAL_MAX_PRESSING_TIME;
        force = min(max(force,0),1);
        __pedalPressed = false;
        
        #ifdef DEBUG_PEDAL
          Serial.print(pressingTime);
          Serial.print('/');
          Serial.print(PEDAL_MAX_PRESSING_TIME);
          Serial.print(" > ");
          Serial.println(force);
        #endif
        
        pressedHandler(force);
      }
      //delay(500);
    }
};


