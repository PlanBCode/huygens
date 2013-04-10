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
        //Serial.print(digitalRead(inputPins[i]));
        //Serial.print(' ');
        if(digitalRead(inputPins[i]) == LOW) 
          current = i;
      }
      //Serial.println(' ');
      if(current != prev) {
        Serial.println(current);
        selectedHandler(current);
      }
      prev = current;
    }
};


