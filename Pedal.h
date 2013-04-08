typedef void (*PedalPressedHandler)(float force);
PedalPressedHandler pedalPressedHandler;

volatile bool _pedalPressing;
volatile bool _pedalPressed;
volatile long int _pedalStartPressingTime;
volatile long int _pedalPressingTime;
    
// pins
int _pedalInputTopPin;
int _pedalInputBottomPin;

void pedalTopChanged() {
  if(digitalRead(_pedalInputTopPin) != LOW) { // if left top
    _pedalStartPressingTime = millis();
    _pedalPressing = true;
  }
}
void pedalBottomChanged() {
  if(_pedalPressing && digitalRead(_pedalInputBottomPin) == LOW) { // if reached bottom
    int pedalEndPressingTime = millis();
    _pedalPressingTime = pedalEndPressingTime-_pedalStartPressingTime;
    _pedalPressing = false;
    _pedalPressed = true;
  }
}

class Pedal {
  
  public: 
    int maxPressingTime;
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
      
      maxPressingTime = 1000; //700
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
      //Serial.print(digitalRead(pedalInputTopPin));
      //Serial.print('\t');
      //Serial.println(digitalRead(pedalInputBottomPin));
      
      if(_pedalPressed && _pedalPressingTime < maxPressingTime) {
        pressingTime = _pedalPressingTime;
        force = float(maxPressingTime-pressingTime)/maxPressingTime;
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


