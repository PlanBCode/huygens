
//#include <wprogram.h>
//#include <wiring_private.h>
//#include <pins_arduino.h>

class Leds{
  
  private: 
    int* objectLedsPins;
    int objectLedsState[NUM_LEDS];
    int numLedsEnabled;

    /*int digitalReadOutputPin(uint8_t pin)
    {
      uint8_t bit = digitalPinToBitMask(pin);
      uint8_t port = digitalPinToPort(pin);
      if (port == NOT_A_PIN) 
        return LOW;
    
      return (*portOutputRegister(port) & bit) ? HIGH : LOW;
    }*/

  public: 
    
    Leds(int* _objectLedsPins) {
    //Leds(int _numRoutes, int _numRouteSteps) {
      objectLedsPins = _objectLedsPins;
      numLedsEnabled = 0; //starting at 1 for start object selection led
      
      for(int i=0;i<NUM_LEDS;i++) {
        int ledPin = objectLedsPins[i];
        pinMode(ledPin, OUTPUT);
        digitalWrite(ledPin,LOW);
        objectLedsState[i] = 0;
      }
      //Serial.println(numLedsEnabled);
    }
    
    void enableObject(int object) {
      //Serial.println(numLedsEnabled);
      int ledPin = objectLedsPins[object];
      if(objectLedsState[object] == 1 || numLedsEnabled > MAX_ENABLED_LEDS) return;
      
      #ifndef DISABLE_LEDS
        digitalWrite(ledPin, HIGH);
      #endif
      objectLedsState[object] = 1;
      numLedsEnabled++;
      
      #ifdef DEBUG_LEDS
        Serial.print("  led: ");
        Serial.println(ledPin);
        //Serial.print(' ');
        //Serial.println(numLedsEnabled);
      #endif
    }
    void disableObject(int object) {
      //Serial.println(numLedsEnabled);
      int ledPin = objectLedsPins[object];
      if(objectLedsState[object] == 0) return;
      digitalWrite(ledPin, LOW);
      objectLedsState[object] = 0;
      numLedsEnabled--;
      
      #ifdef DEBUG_LEDS
        Serial.print("  led disable : ");
        Serial.println(ledPin);
        //Serial.print(' ');
        //Serial.println(numLedsEnabled);
      #endif
    }
    void disableAllObjects() {
      for(int i=0;i<NUM_LEDS;i++) {
        int ledPin = objectLedsPins[i];
        if(objectLedsState[i] == 0) continue;
        digitalWrite(ledPin, LOW);
        objectLedsState[i] = 0;
        numLedsEnabled--;
      }
      #ifdef DEBUG_LEDS
        Serial.println("  led disable all ");
        //Serial.println(numLedsEnabled);
      #endif
    }
    /*void lightStartObjectSelection(int selectedStartObject) {
      for(int i=0;i<NUM_START_OBJECTS;i++) {
        //int startObject = objectLedsPins[]
        digitalWrite(startObjectLedsPins[i],(i == selectedStartObject)? HIGH : LOW);
      }
    }*/
    void update() {
      
    }
  
};


