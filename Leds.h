#define DEBUG_LEDS false

class Leds{
  
  private: 
    int* objectLedsPins;
    int* routeChoiceLedsPins;  
    

    
  public: 
    int current;
    
    Leds(int* _objectLedsPins, int* _routeChoiceLedsPins) {
    //Leds(int _numRoutes, int _numRouteSteps) {
      objectLedsPins = _objectLedsPins;
      routeChoiceLedsPins = _routeChoiceLedsPins;
      
      for(int i=0;i<NUM_OBJECTS;i++) {
        int ledPin = objectLedsPins[i];
        pinMode(ledPin, OUTPUT);
      }
      
      for(int i=0;i<NUM_ROUTES;i++) {
        pinMode(routeChoiceLedsPins[i],OUTPUT);
      }
    }
    
    void enableObject(int object) {
      
      int ledPin = objectLedsPins[object];
      Serial.print("  led: ");
      Serial.println(ledPin);
      digitalWrite(ledPin, HIGH);
    }
    void disableObject(int object) {
      int ledPin = objectLedsPins[object];
      digitalWrite(ledPin, LOW);
    }
    void disableAllObjects() {
      for(int i=0;i<NUM_OBJECTS;i++) {
        int ledPin = objectLedsPins[i];
        digitalWrite(ledPin, LOW);
      }
    }
    void lightRouteSelection(int route) {
      for(int i=0;i<NUM_ROUTES;i++) {
         digitalWrite(routeChoiceLedsPins[i],(i == route)? HIGH : LOW);
      }
    }
    void update() {
      
    }
    
};


