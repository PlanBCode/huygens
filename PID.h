#define damping 0.5
#define SPEED_DAMPING 0.25
#define acceleration 1.1
#define speedLimitedDistance 0.15 //0.25//0.10
//#define minSpeed 0.30

class PID {
  private:
    int resolverPin;
    int speedPin;
    int directionPin;
    int enablePin;
    int fbPin;

  public:
    float currentPos;
    float targetPos;
    float oldPos;
    
    float minPos;
    float maxPos;
    
    float targetSpeed;
    //float currentPos;
    float oldSpeed;
    
    float motorVoltage;
    //int direction;
    int maxFB;
    
    PID(int _resolverPin, int _speedPin, int _directionPin, int _enablePin, int _fbPin) {
        resolverPin = _resolverPin;
        speedPin = _speedPin;
        directionPin = _directionPin;
        enablePin = _enablePin;
        fbPin = _fbPin;
        //direction = 0;
        maxFB = 500; //(525mV per Amp) 525*5/5000*1024
        oldPos = analogRead(resolverPin)/1024.0;
        
        minPos = 0;
        maxPos = 1;
        
//        setPwmFrequency(speedPin,1);

    };
    void gotoPos(float _targetPos, float _speed) {
      #ifdef DEBUG_PID
        Serial.print("  goto: ");
        Serial.print(_targetPos);
        Serial.print(' ');
        Serial.println(_speed);
      #endif  
      targetPos = min(max(_targetPos,0.0),1.0);
      targetSpeed = min(max(_speed,0.0),1.0);
    };
    /*void update() {
        int newDirection;
        int newSpeed;
        
        //Serial.print(analogRead(resolverPin));
        //Serial.print('\t');
        
        float oldPos = currentPos;
        float newPos = analogRead(resolverPin)/1024.0;
        currentPos = (1-damping)*newPos + damping*currentPos;
        currentSpeed = fabs(currentPos - oldPos);

        //Serial.print(currentPos);
        //Serial.print('\t');
        
        //Serial.print(newPos);
        //Serial.print('\t');
        
        //Serial.print(targetPos);
        //Serial.print('\t');
        
        //Serial.print(currentSpeed);
        //Serial.print('\t');

        //float direction
        //float speed;
        newDirection = currentPos < targetPos ? 1 : 0;
        
        if (currentPos == targetPos) newSpeed = 0.0;
        else {
            newSpeed = int(255*currentSpeed * (currentSpeed < 255*targetSpeed ? acceleration : 1.0/acceleration));
            newSpeed = min(max(newSpeed,50), 255);
        }
        //Serial.print(newDirection);
        //Serial.print('\t');
        //Serial.println(newSpeed);
        
        digitalWrite(directionPin, newDirection);
        analogWrite(speedPin, newSpeed);
    }*/
    void update() {
      
      currentPos = analogRead(resolverPin);
      float range = maxPos-minPos;
      currentPos = (currentPos-minPos)/range;
//      currentPos = (1-damping)*currentPos + damping*oldPos;
      
      float distance = fabs(currentPos - targetPos);
      float olddistance = fabs(oldPos - targetPos);
      float currentSpeed = distance - olddistance;
      
      int direction = (currentPos < targetPos)? 1 : 0;
      
      // if drawing too much power, limit motor voltage
      if(analogRead(fbPin) > maxFB) {
        motorVoltage = motorVoltage/acceleration;
      }
      // if position is right, don't fix it
      else if (distance < 0.05) {
        motorVoltage = 0.0;
      }
      // if moving, limit motor voltage (relating to frictionless motor speed) when getting near to target
      else if (currentSpeed > 0.0 && distance < speedLimitedDistance) {
        motorVoltage = motorVoltage * (1.0 - currentSpeed / distance);
      }
      // accelerate or deccelerate towards target speed
      else {
        motorVoltage = motorVoltage * (currentSpeed < targetSpeed ? acceleration : 1.0/acceleration);
        motorVoltage = min(max(motorVoltage, 0.05), 1.0);
      }
      
      int motorPWM = (int)round(255*motorVoltage);
      
      digitalWrite(directionPin, direction? HIGH : LOW);
      analogWrite(speedPin, motorPWM);
     
      oldPos = currentPos;


/*
     #ifdef DEBUG_PID_VERBOSE
        Serial.print(analogRead(resolverPin));
        Serial.print('>');
        Serial.print(currentPos);
        Serial.print('\t');
        Serial.print(targetPos);
        Serial.print('\t');
        Serial.print(distance);
        Serial.print('\t');
        Serial.print(direction);
        Serial.print('\t');
        Serial.print(motorVoltage);
        Serial.print('\t');
        Serial.print(motorPWM);
        Serial.println();
      #endif
*/      


/*
      float newSpeed;
      
      if(distance < speedLimitedDistance) {
//      newSpeed = minSpeed + (1.0 - minSpeed) * targetSpeed * (distance/speedLimitedDistance);
        newSpeed = sqrt(targetSpeed * (distance/speedLimitedDistance));
        //newSpeed = targetSpeed * (distance/speedLimitedDistance);
        #ifdef DEBUG_PID_VERBOSE
          Serial.print('l');
        #endif
      } else {
        #ifdef DEBUG_PID_VERBOSE
          Serial.print('t');
        #endif
//      newSpeed = minSpeed + (1.0 - minSpeed) * targetSpeed;
        newSpeed = targetSpeed;
      }
      #ifdef DEBUG_PID_VERBOSE
        Serial.print(newSpeed);
        Serial.print('>');
      #endif
      
      newSpeed = (1-SPEED_DAMPING)*newSpeed + SPEED_DAMPING*oldSpeed;
      
      #ifdef DEBUG_PID_VERBOSE
        Serial.print(newSpeed);
        Serial.print('>');
      #endif
      //if(newSpeed <= 0.02) newSpeed = 0;
      if(analogRead(fbPin) > maxFB) newSpeed *= 0.5;
      #ifdef DEBUG_PID_VERBOSE
        Serial.print(newSpeed);
        Serial.print('\t');
        Serial.println(analogRead(fbPin));
      #endif
      
      digitalWrite(directionPin, direction? HIGH : LOW);
      analogWrite(speedPin, newSpeed*255);
      // disable motor when it's at target position
      //digitalWrite(enablePin, (distance <= 0.02)? LOW: HIGH); 
      oldSpeed = newSpeed;
      oldPos = currentPos;
      */
    }
};
