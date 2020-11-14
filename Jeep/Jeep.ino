/*
 * Engine control unit for Power Wheels Escalade Dual Motor aka Jeep
 *
 * Author: Michael Congdon
 *
 */

// pin definitions
int throttleInputPin   = A0;     // input pin for throttle pedal

int leftMotorForwardIndicatorPin = 0;     // Left Motor is going forward indicator
int leftMotorReverseIndicatorPin = 1;     // Left Motor is going reverse indicator
int leftMotorForwardOutputPin    = 2;     // Tell Left Motor to go forward pin
int leftMotorReverseOutputPin    = 3;     // Tell Left Motor to go reverse pin
int leftMotorForwardSpeedPin     = 5;     // pwm output left motor Forward Speed
int leftMotorReverseSpeedPin     = 6;     // pwm output left motor Reverse



int ledPin        = 13;     // LED on Arduino Board

// throttle calibration
int throttleValue   = 0;
int throttleMin     = 100;
int throttleMax     = 800;

int leftMotorForward    = 0; 
int leftMotorReverse    = 0; 
int leftMotorForwardSpeed    = 0; 
int leftMotorReverseSpeed    = 0; 

bool IsForward = false; 

void setup() {
  
  // open serial port connection
  Serial.begin(9600); 
   
  //Init pins
  pinMode(leftMotorForwardIndicatorPin, INPUT);
  pinMode(leftMotorReverseIndicatorPin, INPUT);
  pinMode(leftMotorForwardOutputPin, OUTPUT);
  pinMode(leftMotorReverseOutputPin, OUTPUT);
  pinMode(leftMotorForwardSpeedPin, OUTPUT);
  pinMode(leftMotorReverseSpeedPin, OUTPUT);

  // init Left motor (apply parking brake) 
  digitalWrite(leftMotorForwardOutputPin, leftMotorForward); 
  digitalWrite(leftMotorReverseOutputPin, leftMotorReverse);   
  analogWrite(leftMotorForwardSpeedPin, leftMotorForwardSpeed);
  analogWrite(leftMotorReverseSpeedPin, leftMotorReverseSpeed);

  // calibrate min throttle pedal
  throttleValue = analogRead(throttleInputPin);
  throttleMin = throttleValue + 10; 
  

  // light indicator pin when setup complete 
  Serial.println("Setup Complete:");

  // TODO need to have a forward back button of some sort. TBD. 
  IsForward = true;
  }

void loop() {

  // read throttle pedal
  throttleValue = analogRead(throttleInputPin);
  // apply the calibration to the sensor reading
  throttleValue = map(throttleValue, throttleMin, throttleMax, 0, 255);
  // in case the sensor value is outside the range seen during calibration
  throttleValue = constrain(throttleValue, 0, 255);


  // Need to handle reverse... 
  if(throttleValue > 80 ) 
  {
    IsForward = true; 
  } else {
    
    IsForward = false; 
  }
  
  
  // Apply direction var to motor controll 
  if(IsForward) {
    leftMotorForwardSpeed = throttleValue;
    leftMotorReverseSpeed = 0; 
    leftMotorForward = 1;
    leftMotorReverse = 0; 
    Serial.println("Forward"); 
    
  } else {
    // apply brakes 
    Serial.println("Stopped"); 
    leftMotorForwardSpeed = 0;
    leftMotorReverseSpeed = 0; 
    leftMotorForward = 0;
    leftMotorReverse = 0;     
  }

   
  // output to motors
  digitalWrite(leftMotorForwardOutputPin, leftMotorForward); 
  digitalWrite(leftMotorReverseOutputPin, leftMotorReverse);   
  analogWrite(leftMotorForwardSpeedPin, leftMotorForwardSpeed);
  analogWrite(leftMotorReverseSpeedPin, leftMotorReverseSpeed);
  
  delay(10);
  
  //Serial.println(throttleValue);

  Serial.println(leftMotorForwardSpeed);
 
}
