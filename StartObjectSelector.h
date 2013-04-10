class StartObjectSelector {
  
  private: 
    int* inputPins;
    int prev;
    
    typedef void (*StartObjectSelectedHandler)(int startObject);
    StartObjectSelectedHandler selectedHandler;
    
  public: 
    int current;
    
    StartObjectSelector(int* _inputPins, 
                  StartObjectSelectedHandler _selectedHandler) {
                    
      inputPins = _inputPins;
      selectedHandler = _selectedHandler;
      for(int i=0;i<NUM_START_OBJECTS;i++) {
        pinMode(inputPins[i],INPUT);
        digitalWrite(inputPins[i],HIGH);
      }
    }
    
    void update() {
      for(int i=0;i<NUM_START_OBJECTS;i++) {
        #ifdef DEBUG_ROUTE_SELECTOR
          Serial.print(digitalRead(inputPins[i]));
          Serial.print(' ');
        #endif
        if(digitalRead(inputPins[i]) == LOW) 
          current = i;
      }
      #ifdef DEBUG_ROUTE_SELECTOR
        Serial.println(' ');
      #endif
      if(current != prev) {
        selectedHandler(current);
      }
      prev = current;
    }
};


