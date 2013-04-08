typedef void (*PedalPressedHandler)(float force);
PedalPressedHandler pedalPressedHandler;

volatile bool _pedalPressing;
volatile bool _pedalPressed;
volatile long int _pedalStartPressingTime;
volatile long int _pedalPressingTime;
volatile int _pedalMaxPressingTime;

// pins
int _pedalInputTopPin;
int _pedalInputBottomPin;

void pedalTopChanged() {
  //Serial.println('t');
  if(digitalRead(_pedalInputTopPin) == HIGH) { // if left top
    _pedalStartPressingTime = millis();
    _pedalPressing = true;
  } else if(_pedalPressing && !_pedalPressed){ // if it reaches the top again and the pedal wasn't fully pressed down
    int pedalEndPressingTime = millis();
    if(pedalEndPressingTime-_pedalStartPressingTime > 30) { // debounce prevention
      _pedalPressingTime = _pedalMaxPressingTime;
      _pedalPressing = false;
      _pedalPressed = true;
    }
  }
}
void pedalBottomChanged() {
  //Serial.println('b');
  if(_pedalPressing && digitalRead(_pedalInputBottomPin) == LOW) { // if reached bottom
    int pedalEndPressingTime = millis();
    _pedalPressingTime = pedalEndPressingTime-_pedalStartPressingTime;
    _pedalPressing = false;
    _pedalPressed = true;
  }
}

class Pedal {
  
  public: 
    //int maxPressingTime;
    int pressingTime;
    float force;
    
    Pedal(int inputTopPin, int inputBottomPin, 
          int inputTopInterrupt, int inputBottomInterrupt, 
          PedalPressedHandler pressedHandler) {
      _pedalInputTopPin = inputTopPin;
      _pedalInputBottomPin = inputBottomPin;
      pedalPressedHandler = pressedHandler;
      
      _pedalPressing = false;
      _pedalPressed = false;
      _pedalStartPressingTime = 0;
      _pedalPressingTime = 0;
      
      _pedalMaxPressingTime = 700; //700
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
    
    void update() {
      //Serial.print(digitalRead(_pedalInputTopPin));
      //Serial.print('\t');
      //Serial.println(digitalRead(_pedalInputBottomPin));
      
      if(_pedalPressed && _pedalPressingTime <= _pedalMaxPressingTime) {
        pressingTime = _pedalPressingTime;
        force = float(_pedalMaxPressingTime-pressingTime)/_pedalMaxPressingTime;
        force = min(max(force,0),1);
        pedalPressedHandler(force);
        _pedalPressed = false;
        
        /*Serial.print(pressingTime);
        Serial.print('/');
        Serial.print(maxPressingTime);
        Serial.print(" > ");
        Serial.println(force);*/
      }
    }
    
};


