/*
 * Engine control unit for Power Wheels Escalade Dual Motor aka Jeep
 *
 * Author: Michael Congdon
 *
 */

// pin definitions
int throttleInputPin    = A0;    // input pin for throttle pedal

int remote1InputPin    = A1;     // Channel 1 on RC Transmitter (steering)
int remote2InputPin    = A2;     // Channel 2 on RC Transmitter (throttle)
int remote3InputPin    = A3;     // Channel 3 on RC Transmitter (???)

int relayPin          = 11;                 // output pin for aux power relay 

int backwardsPin                = 2;        // reverse switch pin
int rightWheelTurningPin        = 3;        // pin for right (wheel) turning sensor if true turning left
int leftWheelTurningPin         = 4;        // pin for left (wheel) turning sensor if true turning right

int safteyInputPin              = 52;       // input pin for sensor board

int leftMotorForwardIndicatorPin    = 26;      // Left Motor is going forward indicator
int leftMotorBackwardsIndicatorPin  = 27;      // Left Motor is going reverse indicator
int leftMotorForwardOutputPin       = 28;      // Tell Left Motor to go forward pin
int leftMotorBackwardsOutputPin     = 29;      // Tell Left Motor to go reverse pin
int leftMotorForwardSpeedPin        = 9;        // pwm output left motor Forward Speed
int leftMotorBackwardsSpeedPin      = 10;       // pwm output left motor Reverse

int rightMotorForwardIndicatorPin   = 22;      // Right Motor is going forward indicator
int rightMotorBackwardsIndicatorPin = 23;      // Right Motor is going reverse indicator
int rightMotorForwardOutputPin      = 24;      // Tell Right Motor to go forward pin
int rightMotorBackwardsOutputPin    = 25;      // Tell Right Motor to go reverse pin
int rightMotorForwardSpeedPin       = 5;       // pwm output right motor Forward Speed
int rightMotorBackwardsSpeedPin     = 6;       // pwm output right motor Reverse


// throttle calibration
int throttleValue   = 0;
int throttleMin     = 100;
int throttleMax     = 800;

long leftMotorAdjust = 1;     // factor to reduce speed during turn  
long rightMotorAdjust = 1;    // factor to reduce speed during turn 

int leftMotorForwardSpeed     = 0; 
int leftMotorBackwardsSpeed     = 0; 
int rightMotorForwardSpeed    = 0; 
int rightMotorBackwardsSpeed    = 0; 

int SpeedDeltaThreshold = 30;

const int numThrottleReadings = 10;
int throttleReadings[numThrottleReadings];      // the readings from the analog input
int throttleReadIndex = 0;              // the index of the current reading


// initial state stopped
bool IsMoving = false; 
bool IsMovingForward = false; 
bool IsMovingBackwards = false; 
bool IsTurningRight = false; 
bool IsTurningLeft = false; 

bool SafteyPass = false; 
bool DebugMode = true; 

void setup() {
  
  // open serial port connection
  if(DebugMode){
    Serial.begin(9600);  
    Serial.println("**STARTUP****");    
  }
  
  //Init sensor pins
  pinMode(throttleInputPin, INPUT);

  pinMode(remote1InputPin, INPUT);
  pinMode(remote2InputPin, INPUT);
  pinMode(remote3InputPin, INPUT);
  
  pinMode(backwardsPin, INPUT);
  pinMode(rightWheelTurningPin, INPUT);
  pinMode(leftWheelTurningPin, INPUT);
  pinMode(safteyInputPin, INPUT);  
  pinMode(relayPin, OUTPUT);
  
  //Init motor pins    
  pinMode(leftMotorForwardIndicatorPin, INPUT);
  pinMode(leftMotorBackwardsIndicatorPin, INPUT);
  pinMode(rightMotorForwardIndicatorPin, INPUT);
  pinMode(rightMotorBackwardsIndicatorPin, INPUT);

  pinMode(leftMotorForwardOutputPin, OUTPUT);
  pinMode(leftMotorBackwardsOutputPin, OUTPUT);
  pinMode(rightMotorForwardOutputPin, OUTPUT);
  pinMode(rightMotorBackwardsOutputPin, OUTPUT);
  
  pinMode(leftMotorForwardSpeedPin, OUTPUT);
  pinMode(leftMotorBackwardsSpeedPin, OUTPUT);
  pinMode(rightMotorForwardSpeedPin, OUTPUT);
  pinMode(rightMotorBackwardsSpeedPin, OUTPUT);

  // init motor (apply parking brake) 
  digitalWrite(leftMotorForwardOutputPin, LOW); 
  digitalWrite(leftMotorBackwardsOutputPin, LOW);
  digitalWrite(rightMotorForwardOutputPin, LOW); 
  digitalWrite(rightMotorBackwardsOutputPin, LOW);
     
  analogWrite(leftMotorForwardSpeedPin, leftMotorForwardSpeed);
  analogWrite(leftMotorBackwardsSpeedPin, leftMotorBackwardsSpeed);
  analogWrite(rightMotorForwardSpeedPin, rightMotorForwardSpeed);
  analogWrite(rightMotorBackwardsSpeedPin, rightMotorBackwardsSpeed);

  // init throttle read buffer array
  for (int i = 0; i < numThrottleReadings; i++) {
    throttleReadings[i] = 0;
  }
  
  // calibrate min throttle pedal
  throttleValue = analogRead(throttleInputPin);
  throttleMin = throttleValue + 10; 


  // turn on power to dashboard 
  digitalWrite(relayPin, HIGH); 
  
}

void loop() {

  // get Throttle Data
  throttleValue = getThrottleValue(); 
   
  // resolve direction 
  IsMoving            = resolveIsMovingState();
  IsMovingForward     = resolveIsMovingForwardState(); 
  IsMovingBackwards   = resolveIsMovingBackwardsState(); 
  IsTurningRight      = resolveIsTurningRightState(); 
  IsTurningLeft       = resolveIsTurningLeftState(); 

  // Get motor speeds based on direction. 
  leftMotorForwardSpeed     = getLeftMotorForwardSpeed(throttleValue); 
  leftMotorBackwardsSpeed   = getLeftMotorBackwardsSpeed(throttleValue);
  rightMotorForwardSpeed    = getRightMotorForwardSpeed(throttleValue);
  rightMotorBackwardsSpeed  = getRightMotorBackwardsSpeed(throttleValue);
  
  //Saftey sensors
  /*-------------------------------------------------------------*/
  
  // add saftey sensors here. 
  
  // TODO need to filter for random interfance- SaftyPass == false will hard stop the Jeep- motors off. 
  //SafteyPass = digitalRead(safteyInputPin); 
 
  // so much saftey... 
  SafteyPass = true;   

  
  if(!SafteyPass){
    
    // error log 
    if(DebugMode){
      Serial.println("************");
      Serial.println("************");
      Serial.println("*** Saftey Error *** ");
      Serial.println("************");
      Serial.println("************");
    }
    
    // disable motors 
    digitalWrite(leftMotorForwardOutputPin, LOW); 
    digitalWrite(leftMotorBackwardsOutputPin, LOW);   
    digitalWrite(rightMotorForwardOutputPin, LOW); 
    digitalWrite(rightMotorBackwardsOutputPin, LOW);    

    // HARD STOP
    leftMotorForwardSpeed   = 0; 
    leftMotorBackwardsSpeed   = 0;
    rightMotorForwardSpeed  = 0;
    rightMotorBackwardsSpeed  = 0;

    analogWrite(leftMotorForwardSpeedPin, leftMotorForwardSpeed);
    analogWrite(leftMotorBackwardsSpeedPin, leftMotorBackwardsSpeed);
    analogWrite(rightMotorForwardSpeedPin, rightMotorForwardSpeed);
    analogWrite(rightMotorBackwardsSpeedPin, rightMotorBackwardsSpeed);    

  
  } else {
    
    // Move Jeep
    /*-------------------------------------------------------------*/    

    // enable motors
    digitalWrite(leftMotorForwardOutputPin, HIGH); 
    digitalWrite(leftMotorBackwardsOutputPin, HIGH);   
    digitalWrite(rightMotorForwardOutputPin, HIGH); 
    digitalWrite(rightMotorBackwardsOutputPin, HIGH);                      
    
    // apply speed
    analogWrite(leftMotorForwardSpeedPin, leftMotorForwardSpeed);
    analogWrite(leftMotorBackwardsSpeedPin, leftMotorBackwardsSpeed);
    analogWrite(rightMotorForwardSpeedPin, rightMotorForwardSpeed);
    analogWrite(rightMotorBackwardsSpeedPin, rightMotorBackwardsSpeed);    
  }
  
}// loop 





// Get Throttle Reading
/*-------------------------------------------------------------*/
int getThrottleReading()
{
  int throttleReading = 0; 
  
  if(analogRead(remote2InputPin) >= 2){
   
   //Remote Override
   /*-------------------------------------------------------------*/    
    throttleReading = analogRead(remote2InputPin); 
    throttleReading = map(throttleReading, 2, 3, 0, 255);
    
  } else {

   //Throttle Pedal 
   /*-------------------------------------------------------------*/
    throttleReading = analogRead(throttleInputPin);
    throttleReading = map(throttleReading, throttleMin, throttleMax, 0, 255);

  }

  throttleReading = constrain(throttleReading, 0, 255);  
  
  return throttleReading;
}


// Get Throttle Value
/*-------------------------------------------------------------*/
int getThrottleValue()
{ 
  // read throttle store in buffer array
  throttleReadings[throttleReadIndex] = getThrottleReading(); 
  
  // enum read index
  if (throttleReadIndex < numThrottleReadings){
    throttleReadIndex++; 
  } else {
    throttleReadIndex = 0; 
  }
  int throttleTotal = 0; 

  for(int i = 0; i < numThrottleReadings; i++){
    throttleTotal += throttleReadings[i];
  }
  
  return throttleTotal / numThrottleReadings;    
}


// Ease Throttle Change 
/*-------------------------------------------------------------*/
int easeThrottleChange(int oldSpeed, int newSpeed){
    
    if (abs(oldSpeed - newSpeed) > SpeedDeltaThreshold) 
    {
       if(oldSpeed > newSpeed) {
        return (oldSpeed - SpeedDeltaThreshold); 
       } else if (newSpeed > oldSpeed){
        return (oldSpeed + SpeedDeltaThreshold); 
       }
    } 

    return newSpeed;
}


// Get Left Motor Speed
/*-------------------------------------------------------------*/
int getLeftMotorForwardSpeed(int throttle)
{
  if(IsMovingForward)
  {
    if(leftMotorBackwardsSpeed == 0){
      int newSpeed = int(throttle * leftMotorAdjust); 
      return  easeThrottleChange(leftMotorForwardSpeed, newSpeed);
    } else {
     // wait for reverse speed to get to zero
      return 0; 
    }
  } else if (IsMovingBackwards && leftMotorForwardSpeed > 0) {
    
     // if trying to go backwards reduce forward speed to 0; 
     return  easeThrottleChange(leftMotorForwardSpeed, 0);   
    
  }

  return 0; 
}     


// Get Left Motor Backwards Speed 
/*-------------------------------------------------------------*/
int getLeftMotorBackwardsSpeed(int throttle)
{  
  if(IsMovingBackwards)
  {
      if(leftMotorForwardSpeed == 0){
        int newSpeed = int(throttle * leftMotorAdjust); 
        return  easeThrottleChange(leftMotorBackwardsSpeed, newSpeed);
      } else {
        // wait for forward speed to get to zero 
        return 0; 
      }
  } else if (IsMovingForward && leftMotorBackwardsSpeed > 0) {
    
     // if trying to go backwards reduce forward speed to 0; 
     return  easeThrottleChange(leftMotorBackwardsSpeed, 0);   
    
  }
    return 0;
}


// Get Right Motor Forward Speed 
/*-------------------------------------------------------------*/
int getRightMotorForwardSpeed(int throttle)
{
  if(IsMovingForward)
  {
    if(rightMotorBackwardsSpeed == 0){
      int newSpeed = int(throttle * rightMotorAdjust); 
      return  easeThrottleChange(rightMotorForwardSpeed, newSpeed);
    } else {
     // wait for reverse speed to get to zero
      return 0; 
    }
  } else if (IsMovingBackwards && rightMotorForwardSpeed > 0) {
    
     // if trying to go backwards reduce forward speed to 0; 
     return  easeThrottleChange(rightMotorForwardSpeed, 0);   
    
  }

  return 0; 
}

// Get Right Motor Backward Speed 
/*-------------------------------------------------------------*/
int getRightMotorBackwardsSpeed(int throttle)
{
    if(IsMovingBackwards)
  {
      if(rightMotorForwardSpeed == 0){
        int newSpeed = int(throttle * rightMotorAdjust); 
        return  easeThrottleChange(rightMotorBackwardsSpeed, newSpeed);
      } else {
        // wait for forward speed to get to zero 
        return 0; 
      }
  } else if (IsMovingForward && rightMotorBackwardsSpeed > 0) {
    
     // if trying to go backwards reduce forward speed to 0; 
     return  easeThrottleChange(rightMotorBackwardsSpeed, 0);   
    
  }

  return 0;
}


// Resolve Directions Moving 
/*-------------------------------------------------------------*/
bool resolveIsMovingState(){
   
  // This needs to be more sophisticated?  
   return (throttleValue > 20); 
}

bool resolveIsMovingForwardState(){
  if(!IsMoving) {return false;}
  
  // if reverse is active
  if(resolveIsMovingBackwardsState()){
    return false; 
  }
  
  return true;
}

bool resolveIsMovingBackwardsState(){
  if(!IsMoving) {return false;}

  // if reverse pin 
  if(digitalRead(backwardsPin)){
    return true;
  }   

  return false; 
}

bool resolveIsTurningRightState(){
  if(!IsMoving) {return false;} 
  
  //if(!digitalRead(leftWheelTurningPin) && digitalRead(rightWheelTurningPin)){
    //rightMotorAdjust = .7; 
    //return true;
  //}
  
  rightMotorAdjust = 1; 
  return false;
}

bool resolveIsTurningLeftState(){
  if(!IsMoving) {return false;}
  
  //if(!digitalRead(rightWheelTurningPin) && digitalRead(leftWheelTurningPin)){
    //leftMotorAdjust = .7;
    //return true; 
  //}
  
  leftMotorAdjust = 1; 
  return false;    
}
