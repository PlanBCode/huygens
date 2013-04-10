class RouteSelector {
  
  private: 
    int* inputPins;
    int prev;
    
    typedef void (*RouteSelectedHandler)(int route);
    RouteSelectedHandler selectedHandler;
    
  public: 
    int current;
    
    RouteSelector(int* _inputPins, 
                  RouteSelectedHandler _selectedHandler) {
                    
      inputPins = _inputPins;
      selectedHandler = _selectedHandler;
      for(int i=0;i<NUM_ROUTES;i++) {
        pinMode(inputPins[i],INPUT);
        digitalWrite(inputPins[i],HIGH);
      }
    }
    
    void update() {
      for(int i=0;i<NUM_ROUTES;i++) {
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


