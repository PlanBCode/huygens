class RouteSelector {
  
  private: 
    int* inputPins;
    int* ledsPins;
    int numRoutes;
    int prev;
    
  public: 
    int current;
    
    RouteSelector(int* _inputPins, int* _ledsPins, int _numRoutes) {
      inputPins = _inputPins;
      ledsPins = _ledsPins;
      numRoutes = _numRoutes;
      
      for(int i=0;i<numRoutes;i++) {
        pinMode(inputPins[i],INPUT);
        digitalWrite(inputPins[i],HIGH);
      }
    }
    
    void update() {
      for(int i=0;i<numRoutes;i++) {
        //Serial.print(digitalRead(routeInputPins[i]));
        //Serial.print(' ');
        if(digitalRead(inputPins[i]) == LOW) 
          current = i;
      }
      //Serial.println(' ');
      if(current != prev) {
        for(int i=0;i<numRoutes;i++) {
          digitalWrite(ledsPins[i],(current == i)? HIGH : LOW);
        }
      }
      prev = current;
    }
};


