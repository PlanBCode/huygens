#include "PID.h"

class TimePointer {
  
  private: 
    PID pid;
  
  public: 
    int maxPressingTime;
    int pressingTime;
    float force;
    
    TimePointer(int _resolverPin, int _speedPin, int _directionPin, int _enablePin, int _fbPin) {
      pid = new PID( _resolverPin, _speedPin, _directionPin, _enablePin, _fbPin);
      
    }
    
    void update() {
      
    }
};


