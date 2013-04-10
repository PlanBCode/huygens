

// pins
int __pedalInputTopPin;
int __pedalInputBottomPin;

volatile bool __pedalPressing;
volatile bool __pedalPressed;
volatile long unsigned int __pedalStartPressingTime;
volatile long unsigned int __pedalEndPressingTime;
volatile long unsigned int __pedalPressingTime;
volatile int __pedalMaxPressingTime;

void pedalTopChanged() {
  //Serial.println('t');
  if(digitalRead(__pedalInputTopPin) == HIGH) { // if left top
    __pedalStartPressingTime = millis();
    __pedalPressing = true;
  }
  // TODO stops working after a while:
  /* else if(__pedalPressing && !__pedalPressed){ // if it reaches the top again and the pedal wasn't fully pressed down
    //__pedalEndPressingTime = millis();
    //Serial.println(__pedalEndPressingTime-__pedalStartPressingTime);
    if(millis()-__pedalStartPressingTime > 30) { // debounce prevention
      __pedalPressingTime = __pedalMaxPressingTime*0.8;
      __pedalPressing = false;
      __pedalPressed = true;
    }
  }*/
}
void pedalBottomChanged() {
  //Serial.println('b');
  if(__pedalPressing && digitalRead(__pedalInputBottomPin) == LOW) { // if reached bottom
    int pedalEndPressingTime = millis();
    __pedalPressingTime = pedalEndPressingTime-__pedalStartPressingTime;
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
      
      __pedalMaxPressingTime = 700; //700
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
      
      if(__pedalPressed && __pedalPressingTime <= __pedalMaxPressingTime) {
        pressingTime = __pedalPressingTime;
        force = float(__pedalMaxPressingTime-pressingTime)/__pedalMaxPressingTime;
        force = min(max(force,0),1);
        __pedalPressed = false;
        pressedHandler(force);
        
        #ifdef DEBUG_PEDAL
          Serial.print(pressingTime);
          Serial.print('/');
          Serial.print(__pedalMaxPressingTime);
          Serial.print(" > ");
          Serial.println(force);
        #endif
      }
      //delay(500);
    }
    
};


