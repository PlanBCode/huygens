
class Pedal {
  
  private: 
    typedef void (*PedalPressedHandler)(float force);
    PedalPressedHandler pressedHandler;
    int inputTopPin;
    int inputBottomPin;
    
  public: 
    float force;
    
    Pedal(int _inputTopPin, int _inputBottomPin, PedalPressedHandler _pressedHandler) {
      
      inputTopPin = _inputTopPin;
      inputBottomPin = _inputBottomPin;
      pressedHandler = _pressedHandler;
      
      pinMode(inputTopPin,INPUT);
      pinMode(inputBottomPin,INPUT);
      
      // enable pull-up resistors
      digitalWrite(inputTopPin,HIGH);
      digitalWrite(inputBottomPin,HIGH);
      
    }
    void update() {
      #ifdef DEBUG_PEDAL_VERBOSE
        Serial.println(digitalRead(inputTopPin));
        //Serial.print('\t');
        //Serial.println(digitalRead(inputBottomPin));
      #endif
      
      if(digitalRead(inputTopPin) == HIGH) {
        force = 1.0f;
        pressedHandler(force);
      }
    }
};


