/* === How to use ===
 * include "PID_v1.h" in main code
 * instantiate class using the constructor function:
 *   Tijdwijzer(int _resolverPin, int _speedPin, int _directionPin, int _enablePin, int _slewPin, int _feedbackPin, float _minPos, float _maxPos);
 * set the below constants to optimal servo settings (use pragma DEBUG_PID to tune)
 * call servo update function at faster rate than constant cycletime
 * send motor to position using gotoPos(position[0-1], speed[0-1])
 * use pragma DEBUG_PID_VERBOSE for additional debug feedback
 */

#define Kp 4000 // 2000 // 4000   //Determines how aggressively the PID reacts to the current amount of error (Proportional) (double >=0)
#define Ki 100 // 50 //150//110 //100// 0               // Determines how aggressively the PID reacts to error over time (Integral) (double>=0)
#define Kd 500 //200// 500 //300             // Determines how aggressively the PID reacts to the change in error (Derivative) (double>=0)
#define Vmin 100 //120 //115 //120 //100
#define Vmax 200
#define Imax 500 // 525mV per Amp (max power supply 5V*0.525* 5.0/1024 = 537)
#define cycletime 5 // ms between PID computations
#define resolverPrecision 0.05//0.05

class Tijdwijzer {
  private:
    int resolverPin;
    int speedPin;
    int directionPin;
    int enablePin;
    int slewPin;
    int feedbackPin;
    double Setpoint;
    double Input;
    double Output;
    
    PID* servo;

  public:
    float minPos;
    float maxPos;
    
    Tijdwijzer(int _resolverPin, int _speedPin, int _directionPin, int _enablePin, int _slewPin, int _feedbackPin, float _minPos, float _maxPos) {
      resolverPin = _resolverPin;
      speedPin = _speedPin;
      directionPin = _directionPin;
      enablePin = _enablePin;
      slewPin = _slewPin;
      feedbackPin = _feedbackPin;
      minPos = _minPos;
      maxPos = _maxPos;
      
      // setup motor
      pinMode(feedbackPin, INPUT);
      digitalWrite(enablePin, HIGH);
      digitalWrite(slewPin, HIGH);
      TCCR1B=(TCCR1B & 0xF8) | 0x01;      // set pwm frequency above audible range
      
      // setup resolver
      pinMode(resolverPin, INPUT);
      Input = (analogRead(resolverPin) - minPos)/(maxPos - minPos);
      
      // setup PID
      servo = new PID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
      servo->SetOutputLimits(-Vmax, Vmax);
      servo->SetSampleTime(5);
      servo->SetTunings(Kp, Ki, Kd);
      
      //turn the PID on
      Setpoint = 0.0;
      servo->SetMode(AUTOMATIC);
    }
    void gotoPos(float position, float speed) {
      int pwm = int(map(max(min(speed, 1.0), 0.0), 0.0, 1.0, Vmin, Vmax));

      #ifdef DEBUG_PID
        Serial.print("  goto: ");
        Serial.print(position);
        Serial.print(' @ ');
        Serial.print(speed);
        Serial.print(' (');
        Serial.print(pwm);
        Serial.println(')');
      #endif

      servo->SetOutputLimits(-pwm,pwm);
      Setpoint = max(min(position, 1.0), 0.0);
    }
    float getPosition() {
      return Input;
    }
    float getTargetPosition() {
      return Setpoint;
    }
    void update() {
      Input = float(analogRead(resolverPin) - minPos)/(maxPos - minPos);
      servo->Compute();
      int pwm = (fabs(Input - Setpoint) < resolverPrecision ? 0 : round(abs(Output)));
      int dir = Output < 0 ? 0 : 1;
      digitalWrite(directionPin, dir);
      analogWrite(speedPin, pwm);

      #ifdef DEBUG_PID
	int feedback = analogRead(feedbackPin);
        if ( feedback > Imax) {
          Serial.print("overload: ");
          Serial.println(feedbackPin);
	    }
      #endif

      #ifdef DEBUG_PID_VERBOSE
        if (millis()%500 == 0) {
          Serial.print("Input ");
          Serial.print(Input);
          Serial.print(" Setpoint ");
          Serial.print(Setpoint);
          Serial.print(" Output ");
          Serial.print(Output);
          Serial.print(" Direction ");
          Serial.print(dir);
          Serial.print(" PWM ");
          Serial.print(pwm);
          Serial.println();
        }
      #endif
  }
};

