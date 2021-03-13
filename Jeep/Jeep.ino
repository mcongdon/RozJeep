/*
 * Engine control unit for Power Wheels Escalade Dual Motor aka Jeep
 *
 * Author: Michael Congdon
 *
 */

// pin definitions
int throttleInputPin    = A0;    // input pin for throttle pedal

int remote1InputPin    = A1;     // Channel 1 on RC Transmitter (steering)
int remote2InputPin    = A5;     // Channel 2 on RC Transmitter (throttle)
int remote3InputPin    = A9;     // Channel 3 on RC Transmitter (???)

int relayPin          = 30;                 // output pin for aux power relay 

int backwardsPin                = 31;        // reverse switch pin
int rightWheelTurningPin        = 34;        // pin for right (wheel) turning sensor if true turning left
int leftWheelTurningPin         = 35;        // pin for left (wheel) turning sensor if true turning right

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
int rightMotorForwardSpeedPin       = 6;       // pwm output right motor Forward Speed
int rightMotorBackwardsSpeedPin     = 7;       // pwm output right motor Reverse

int leftMotorForwardSpeed     = 0; 
int leftMotorBackwardsSpeed   = 0; 
int rightMotorForwardSpeed    = 0; 
int rightMotorBackwardsSpeed  = 0; 

long leftMotorAdjust = 1;     // factor to reduce speed during turn  
long rightMotorAdjust = 1;    // factor to reduce speed during turn 

int SpeedDeltaThreshold = 20;

// throttle vars 
int throttleValue   = 0;

// buffer array for throttle pedal input 
const int numThrottlePedalReadings = 20;
int throttlePedalReadings[numThrottlePedalReadings];      // the readings from the analog input
int throttlePedalReadIndex = 0;                      // the index of the current reading
int throttlePedalMin     = 0;
int throttlePedalMax     = 800;

// buffer array for remote throttle control input 
const int numRemoteThrottleReadings = 30;
int remoteThrottleReadings[numRemoteThrottleReadings];  // the readings from the analog input
int remoteThrottleReadIndex = 0;                        // the index of the current reading
int remoteThrottleMin     = 0;
int remoteThrottleMax     = 400;

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
  
  //Init throttle pins
  pinMode(throttleInputPin, INPUT);
  pinMode(remote1InputPin, INPUT);
  pinMode(remote2InputPin, INPUT);  
  pinMode(remote3InputPin, INPUT);
  
  pinMode(relayPin, OUTPUT);
  pinMode(backwardsPin, INPUT);

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
  for (int i = 0; i < numThrottlePedalReadings; i++) {
    throttlePedalReadings[i] = 0;
  }

  // init remote throttle read buffer array
  for (int i = 0; i < numRemoteThrottleReadings; i++) {
    remoteThrottleReadings[i] = 0;
  }
  
  // calibrate min throttle pedal
  int initThrottlePedalValue = analogRead(throttleInputPin);
  throttlePedalMin = initThrottlePedalValue + 20; 

  
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



// Get Throttle Value
/*-------------------------------------------------------------*/
int getThrottleValue()
{ 
 // if Remote has value return 
 int remoteThrottleValue = getRemoteThrottleValue();
 int throttlePedalValue = getThrottlePedalValue(); 

 if(DebugMode){
    Serial.print("Throttle Pedal:\t"); 
    Serial.print(throttlePedalValue); 
    Serial.print("Remote Throttle:\t"); 
    Serial.println(remoteThrottleValue); 
  }
 
 if( remoteThrottleValue > 0) {
  return remoteThrottleValue; 
 }
 
 return throttlePedalValue;
}


// Get Throttle Pedal Value
/*-------------------------------------------------------------*/
int getThrottlePedalValue()
{

  // get analog data from throttle pedal  
  int throttlePedalReading = analogRead(throttleInputPin);
  
  // map to calibration values 
  throttlePedalReading = map(throttlePedalReading, throttlePedalMin, throttlePedalMax, 0, 255);
  throttlePedalReading = constrain(throttlePedalReading, 0, 255);  
  
  // store in buffer array
  throttlePedalReadings[throttlePedalReadIndex] = throttlePedalReading; 

  // iterate buffer index
  if (throttlePedalReadIndex < numThrottlePedalReadings){
    throttlePedalReadIndex++; 
  } else {
    throttlePedalReadIndex = 0; 
  }

  // return average of buffer array
  int throttlePedalTotal = 0; 

  for(int i = 0; i < numThrottlePedalReadings; i++){
    throttlePedalTotal += throttlePedalReadings[i];
  }
  
  return throttlePedalTotal / numThrottlePedalReadings;    
}


// Get Remote Throttle Value
/*-------------------------------------------------------------*/
int getRemoteThrottleValue()
{
 
  int remoteThrottleReading = pulseIn(remote2InputPin, HIGH, 25000); // throttle 
  
  // map to calibration values 
  remoteThrottleReading = remoteThrottleReading - 1600; 

  // make 0 if neg value
  if(remoteThrottleReading < 0){
    remoteThrottleReading = 0; 
  }
  
  remoteThrottleReading = map(remoteThrottleReading, remoteThrottleMin, remoteThrottleMax, 0, 255);
  
  // store in buffer array
  remoteThrottleReadings[remoteThrottleReadIndex] = remoteThrottleReading; 

  // iterate buffer index
  if (remoteThrottleReadIndex < numRemoteThrottleReadings){
    remoteThrottleReadIndex++; 
  } else {
    remoteThrottleReadIndex = 0; 
  }

  
  // return average of buffer array
  int remoteThrottleTotal = 0; 

  for(int i = 0; i < numRemoteThrottleReadings; i++){
    remoteThrottleTotal += remoteThrottleReadings[i];
  }

  return remoteThrottleTotal / numRemoteThrottleReadings;       
 
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
  return !digitalRead(backwardsPin);
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
