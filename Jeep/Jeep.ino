/*
 * Engine control unit for Power Wheels Escalade Dual Motor aka Jeep
 *
 * Author: Michael Congdon
 *
 */

// pin definitions
int throttleInputPin    = A0;    // input pin for throttle pedal

int relayPin          = 30;                 // output pin for aux power relay 

int backwardsPin                = 31;        // reverse switch pin
int rightWheelTurningPin        = 49;        // pin for right (wheel) turning sensor if true turning left
int leftWheelTurningPin         = 47;        // pin for left (wheel) turning sensor if true turning right

int safteyInputPin              = 52;       // input pin for sensor board


int leftMotorForwardOutputPin       = 28;      // Tell Left Motor to go forward pin
int leftMotorBackwardsOutputPin     = 29;      // Tell Left Motor to go reverse pin
int leftMotorForwardSpeedPin        = 10;        // pwm output left motor Forward Speed
int leftMotorBackwardsSpeedPin      = 9;       // pwm output left motor Reverse

int rightMotorForwardOutputPin      = 24;      // Tell Right Motor to go forward pin
int rightMotorBackwardsOutputPin    = 25;      // Tell Right Motor to go reverse pin
int rightMotorForwardSpeedPin       = 6;       // pwm output right motor Forward Speed
int rightMotorBackwardsSpeedPin     = 5;       // pwm output right motor Reverse

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


// initial state stopped
bool IsMovingBackwards = false;
 
bool IsTurningRight = false; 
bool IsTurningLeft = false; 

bool SafteyPass = false; 
bool DebugMode = false; 

void setup() {
  
  // open serial port connection
  if(DebugMode){
    Serial.begin(9600);  
    Serial.println("**STARTUP****"); 
  }
  
  //Init throttle pins
  pinMode(throttleInputPin, INPUT);
  
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
     
  analogWrite(leftMotorForwardSpeedPin, 0);
  analogWrite(leftMotorBackwardsSpeedPin, 0);
  analogWrite(rightMotorForwardSpeedPin, 0);
  analogWrite(rightMotorBackwardsSpeedPin, 0);

  // init throttle read buffer array
  for (int i = 0; i < numThrottlePedalReadings; i++) {
    throttlePedalReadings[i] = 0;
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

    analogWrite(leftMotorForwardSpeedPin, 0);
    analogWrite(leftMotorBackwardsSpeedPin, 0);
    analogWrite(rightMotorForwardSpeedPin, 0);
    analogWrite(rightMotorBackwardsSpeedPin, 0);    

  
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

 if(DebugMode){
    Serial.print("leftMotorForwardSpeed:\t"); 
    Serial.print(leftMotorForwardSpeed); 
    Serial.print("leftMotorBackwardsSpeed:\t"); 
    Serial.print(leftMotorBackwardsSpeed); 
    Serial.print("rightMotorForwardSpeed:\t"); 
    Serial.print(rightMotorForwardSpeed); 
    Serial.print("rightMotorBackwardsSpeed:\t"); 
    Serial.print(rightMotorBackwardsSpeed); 
    Serial.println("); 
  }

  
}// loop 



// Get Throttle Value
/*-------------------------------------------------------------*/
int getThrottleValue()
{ 

 int throttlePedalValue = getThrottlePedalValue(); 
 
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

  // if pedal is more than half way down set to full to eliminate noise 
  if (throttlePedalReading > 170){
    throttlePedalReading = 255; 
  }
  
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


// Ease Throttle Change 
/*-------------------------------------------------------------*/
int easeThrottleChange(int oldSpeed, long newSpeed){
    int returnSpeed = int(newSpeed); 
    if (abs(oldSpeed - returnSpeed) > SpeedDeltaThreshold) 
    {
       if(oldSpeed > returnSpeed) {
        return (oldSpeed - SpeedDeltaThreshold); 
       } else if (newSpeed > oldSpeed){
        return (oldSpeed + SpeedDeltaThreshold); 
       }
    } 

    return returnSpeed;
}


// Get Left Motor Speed
/*-------------------------------------------------------------*/
int getLeftMotorForwardSpeed(int throttle)
{
  if(!IsMovingBackwards)
  {
    if(leftMotorBackwardsSpeed == 0){
      long newSpeed = long(throttle) * leftMotorAdjust; 
      return  easeThrottleChange(leftMotorForwardSpeed, newSpeed);
    } else {
     // wait for reverse speed to get to zero
      return 0; 
    }
  } else if (leftMotorForwardSpeed > 0) {
    
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
        long newSpeed = long(throttle) * leftMotorAdjust; 
        return  easeThrottleChange(leftMotorBackwardsSpeed, newSpeed);
      } else {
        // wait for forward speed to get to zero 
        return 0; 
      }
  } else if (leftMotorBackwardsSpeed > 0) {
    
     // if trying to go backwards reduce forward speed to 0; 
     return  easeThrottleChange(leftMotorBackwardsSpeed, 0);   
    
  }
    return 0;
}


// Get Right Motor Forward Speed 
/*-------------------------------------------------------------*/
int getRightMotorForwardSpeed(int throttle)
{
  if(!IsMovingBackwards)
  {
    if(rightMotorBackwardsSpeed == 0){
      long newSpeed = long(throttle) * rightMotorAdjust; 
      return easeThrottleChange(rightMotorForwardSpeed, newSpeed);
    } else {
     // wait for reverse speed to get to zero
      return 0; 
    }
  } else if (rightMotorForwardSpeed > 0) {
    
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
        long newSpeed = long(throttle) * rightMotorAdjust; 
        return  easeThrottleChange(rightMotorBackwardsSpeed, newSpeed);
      } else {
        // wait for forward speed to get to zero 
        return 0; 
      }
  } else if (rightMotorBackwardsSpeed > 0) {
    
     // if trying to go backwards reduce forward speed to 0; 
     return  easeThrottleChange(rightMotorBackwardsSpeed, 0);   
    
  }

  return 0;
}


// Resolve Directions Moving 
/*-------------------------------------------------------------*/
bool resolveIsMovingBackwardsState(){
  // if reverse pin 
  return digitalRead(backwardsPin);
}

bool resolveIsTurningRightState(){
  
  if(!digitalRead(leftWheelTurningPin) && digitalRead(rightWheelTurningPin)){
    rightMotorAdjust = 0.5; 
    return true;
  }
  
  rightMotorAdjust = 1; 
  return false;
}

bool resolveIsTurningLeftState(){
  
  if(!digitalRead(rightWheelTurningPin) && digitalRead(leftWheelTurningPin)){
    leftMotorAdjust = 0.5;
    return true; 
  }
  
  leftMotorAdjust = 1; 
  return false;    
}
