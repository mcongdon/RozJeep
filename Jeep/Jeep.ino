/*
 * Engine control unit for Power Wheels Escalade Dual Motor aka Jeep
 *
 * Author: Michael Congdon
 *
 */

// pin definitions
int throttleInputPin    = A0;     // input pin for throttle pedal


int relayPin          = 11;               // output pin for aux power relay 

int reversePin                  = 2;      // reverse switch pin
int rightWheelTurningPin        = 3;      // pin for right (wheel) turning sensor if true turning left
int leftWheelTurningPin         = 4;      // pin for left (wheel) turning sensor if true turning right

int safteyInputPin              = 50;               // input pin for sensor board

int leftMotorForwardIndicatorPin = 26;      // Left Motor is going forward indicator
int leftMotorReverseIndicatorPin = 27;      // Left Motor is going reverse indicator
int leftMotorForwardOutputPin    = 28;      // Tell Left Motor to go forward pin
int leftMotorReverseOutputPin    = 29;      // Tell Left Motor to go reverse pin
int leftMotorForwardSpeedPin     = 10;       // pwm output left motor Forward Speed
int leftMotorReverseSpeedPin     = 9;       // pwm output left motor Reverse

int rightMotorForwardIndicatorPin = 22;      // Right Motor is going forward indicator
int rightMotorReverseIndicatorPin = 23;      // Right Motor is going reverse indicator
int rightMotorForwardOutputPin    = 24;      // Tell Right Motor to go forward pin
int rightMotorReverseOutputPin    = 25;      // Tell Right Motor to go reverse pin
int rightMotorForwardSpeedPin     = 6;       // pwm output right motor Forward Speed
int rightMotorReverseSpeedPin     = 5;       // pwm output right motor Reverse


// throttle calibration
int throttleValue   = 0;
int throttleMin     = 100;
int throttleMax     = 800;

long leftMotorAdjust = 1;     // factor to reduce speed during turn  
long rightMotorAdjust = 1;    // factor to reduce speed during turn 

int leftMotorForwardSpeed     = 0; 
int leftMotorReverseSpeed     = 0; 
int rightMotorForwardSpeed    = 0; 
int rightMotorReverseSpeed    = 0; 

// initial state stopped
bool IsMoving = false; 
bool SafteyPass = false; 

void setup() {
  
  // open serial port connection
  //Serial.begin(9600); 
   
  //Init sensor pins
  pinMode(throttleInputPin, INPUT);
  pinMode(reversePin, INPUT);
  pinMode(rightWheelTurningPin, INPUT);
  pinMode(leftWheelTurningPin, INPUT);
  pinMode(safteyInputPin, INPUT);  
  pinMode(relayPin, OUTPUT);
  
  //Init motor pins    
  pinMode(leftMotorForwardIndicatorPin, INPUT);
  pinMode(leftMotorReverseIndicatorPin, INPUT);
  pinMode(rightMotorForwardIndicatorPin, INPUT);
  pinMode(rightMotorReverseIndicatorPin, INPUT);

  pinMode(leftMotorForwardOutputPin, OUTPUT);
  pinMode(leftMotorReverseOutputPin, OUTPUT);
  pinMode(rightMotorForwardOutputPin, OUTPUT);
  pinMode(rightMotorReverseOutputPin, OUTPUT);
  
  pinMode(leftMotorForwardSpeedPin, OUTPUT);
  pinMode(leftMotorReverseSpeedPin, OUTPUT);
  pinMode(rightMotorForwardSpeedPin, OUTPUT);
  pinMode(rightMotorReverseSpeedPin, OUTPUT);

  // init motor (apply parking brake) 
  digitalWrite(leftMotorForwardOutputPin, LOW); 
  digitalWrite(leftMotorReverseOutputPin, LOW);
  digitalWrite(rightMotorForwardOutputPin, LOW); 
  digitalWrite(rightMotorReverseOutputPin, LOW);
     
  analogWrite(leftMotorForwardSpeedPin, leftMotorForwardSpeed);
  analogWrite(leftMotorReverseSpeedPin, leftMotorReverseSpeed);
  analogWrite(rightMotorForwardSpeedPin, rightMotorForwardSpeed);
  analogWrite(rightMotorReverseSpeedPin, rightMotorReverseSpeed);

  // calibrate min throttle pedal
  throttleValue = analogRead(throttleInputPin);
  throttleMin = throttleValue + 10; 

  // turn on power to dashboard 
  digitalWrite(relayPin, HIGH); 
  
}

void loop() {

  //Throttle
  /*-------------------------------------------------------------*/
  // read throttle pedal
  throttleValue = analogRead(throttleInputPin);
  // apply the calibration to the sensor reading
  throttleValue = map(throttleValue, throttleMin, throttleMax, 0, 255);
  // in case the sensor value is outside the range seen during calibration
  throttleValue = constrain(throttleValue, 0, 255);


  //Saftey sensors
  /*-------------------------------------------------------------*/
  
  // add saftey sensors here. 

  //SafteyPass = digitalRead(safteyInputPin); 
  IsMoving = (throttleValue > 50);   


  //Motor Control - find speed value for each motor
  /*-------------------------------------------------------------*/
  
  if(IsMoving) {
    
    //Direction Sensors
    /*-------------------------------------------------------------*/
    // right wheel sensor active jeep is turning left when moving forward. 
    if(digitalRead(rightWheelTurningPin)){
      leftMotorAdjust = .7; 
    } else {
      leftMotorAdjust = 1; 
    }
  
    // left wheel sensor active jeep is turning right when moving forward. 
    if(digitalRead(leftWheelTurningPin)){
      rightMotorAdjust = .7; 
    } else {
      rightMotorAdjust = 1; 
    }

    
    // if the (poorly named) reverse switch is active jeep is moving forward
    
    /* removed to test reverse switch
     *  
     if (digitalRead(reversePin)){
      
      leftMotorForwardSpeed   = (throttleValue * leftMotorAdjust);
      leftMotorReverseSpeed   = 0; 
    
      rightMotorForwardSpeed  = (throttleValue * rightMotorAdjust);
      rightMotorReverseSpeed  = 0; 
    
    // is moving backwards
    } else {
      leftMotorReverseSpeed   = (throttleValue * leftMotorAdjust);
      leftMotorForwardSpeed   = 0; 
    
      rightMotorReverseSpeed  = (throttleValue * rightMotorAdjust);
      rightMotorForwardSpeed  = 0; 
    }
  
  */
    // test values for direction sensor- 
    leftMotorForwardSpeed   = (throttleValue * leftMotorAdjust);
    leftMotorReverseSpeed   = 0; 
    
    rightMotorForwardSpeed  = (throttleValue * rightMotorAdjust);
    rightMotorReverseSpeed  = 0; 
    // remove above 

    // enable motors 
    digitalWrite(leftMotorForwardOutputPin, HIGH); 
    digitalWrite(leftMotorReverseOutputPin, HIGH);   
    digitalWrite(rightMotorForwardOutputPin, HIGH); 
    digitalWrite(rightMotorReverseOutputPin, HIGH);      
      
    // apply speed
    analogWrite(leftMotorForwardSpeedPin, leftMotorForwardSpeed);
    analogWrite(leftMotorReverseSpeedPin, leftMotorReverseSpeed);
    analogWrite(rightMotorForwardSpeedPin, rightMotorForwardSpeed);
    analogWrite(rightMotorReverseSpeedPin, rightMotorReverseSpeed);

   
  } else { 
    
    // if not IsMoving
    
    // apply brakes 
    leftMotorForwardSpeed = 0;
    leftMotorReverseSpeed = 0; 
    rightMotorForwardSpeed = 0;
    rightMotorReverseSpeed = 0; 
    
    // enable motors 
    digitalWrite(leftMotorForwardOutputPin, LOW); 
    digitalWrite(leftMotorReverseOutputPin, LOW);   
    digitalWrite(rightMotorForwardOutputPin, LOW); 
    digitalWrite(rightMotorReverseOutputPin, LOW);      
      
    // apply speed
    analogWrite(leftMotorForwardSpeedPin, leftMotorForwardSpeed);
    analogWrite(leftMotorReverseSpeedPin, leftMotorReverseSpeed);
    analogWrite(rightMotorForwardSpeedPin, rightMotorForwardSpeed);
    analogWrite(rightMotorReverseSpeedPin, rightMotorReverseSpeed);
  }
  
}
