/*
 * Engine control unit for Power Wheels Escalade Dual Motor aka Jeep
 *
 * Author: Michael Congdon
 *
 */

// pin definitions
int throttleInputPin    = A0;     // input pin for throttle pedal


int relayPin          = 11;                 // output pin for aux power relay 

int reversePin                  = 2;        // reverse switch pin
int rightWheelTurningPin        = 3;        // pin for right (wheel) turning sensor if true turning left
int leftWheelTurningPin         = 4;        // pin for left (wheel) turning sensor if true turning right

int safteyInputPin              = 52;       // input pin for sensor board

int leftMotorForwardIndicatorPin = 26;      // Left Motor is going forward indicator
int leftMotorReverseIndicatorPin = 27;      // Left Motor is going reverse indicator
int leftMotorForwardOutputPin    = 28;      // Tell Left Motor to go forward pin
int leftMotorReverseOutputPin    = 29;      // Tell Left Motor to go reverse pin
int leftMotorForwardSpeedPin     = 9;      // pwm output left motor Forward Speed
int leftMotorReverseSpeedPin     = 10;       // pwm output left motor Reverse

int rightMotorForwardIndicatorPin = 22;      // Right Motor is going forward indicator
int rightMotorReverseIndicatorPin = 23;      // Right Motor is going reverse indicator
int rightMotorForwardOutputPin    = 24;      // Tell Right Motor to go forward pin
int rightMotorReverseOutputPin    = 25;      // Tell Right Motor to go reverse pin
int rightMotorForwardSpeedPin     = 5;       // pwm output right motor Forward Speed
int rightMotorReverseSpeedPin     = 6;       // pwm output right motor Reverse


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
bool DebugMode = true; 

void setup() {
  
  // open serial port connection
  if(DebugMode){
    Serial.begin(9600);  
    Serial.println("**STARTUP****");    
  }
  
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
  
  if(DebugMode){
    Serial.println("**STARTUP END****");    
  } 
}

void loop() {

  //Throttle
  /*-------------------------------------------------------------*/
  // read throttle pedal
  throttleValue = analogRead(throttleInputPin);
  if(DebugMode){
    Serial.print("Throttle RAW: ");
    Serial.print(throttleValue);
    Serial.print(",");
  }
  // apply the calibration to the sensor reading
  throttleValue = map(throttleValue, throttleMin, throttleMax, 0, 255);
  if(DebugMode){
    Serial.print("Throttle Map: ");
    Serial.print(throttleValue);
    Serial.print(",");
  }
  
  // in case the sensor value is outside the range seen during calibration
  throttleValue = constrain(throttleValue, 0, 255);
  if(DebugMode){
    Serial.print("Throttle Constrain: ");
    Serial.print(throttleValue);
    Serial.print(" : ");
  }  

  
  //Saftey sensors
  /*-------------------------------------------------------------*/
  
  // add saftey sensors here. 
  
  // TODO need to filter for random interfance- SaftyPass == false will hard stop the Jeep- motors off. 
  //SafteyPass = digitalRead(safteyInputPin); 
 
  SafteyPass = true; 

  
  // This needs to be more sophisticated...
  // IsMoving = (throttleValue > 50);   
  IsMoving = true;

  
  //Motor Control - find speed value for each motor
  /*-------------------------------------------------------------*/
  
  if(IsMoving) {

    if(DebugMode){
      Serial.print("Is Moving: ");
    }  



    // Resolve Direction Sensors
    /*-------------------------------------------------------------*/
    // ** note the mechanical design prevents both right wheel sensor and left wheel sensor to both be true at the same time
    // ** exception should be created and handled for such an event, indicating credical mechanical failure 
    
    // if right wheel sensor active jeep is turning left. 
    if(!digitalRead(rightWheelTurningPin)){

      if(DebugMode){
        Serial.print("LEFT AND ");
      }        
      // is turning left - slow left motor 
      leftMotorAdjust = .7; 

    } else {
      
      // not going left
      leftMotorAdjust = 1; 
    }
  
    // left wheel sensor active jeep is turning right. 
    if(!digitalRead(leftWheelTurningPin)){
       
       if(DebugMode){
        Serial.print("RIGHT AND ");
      }                 
      // is turning right - slow right motor 
      rightMotorAdjust = .7; 

    } else {
      
      // not going right
      rightMotorAdjust = 1; 
    }

    
    // Resolve Forward/Reverse
    /*-------------------------------------------------------------*/
    // if reverse sensor active Jeep is moving backwards
    if (!digitalRead(reversePin)){

      if(DebugMode){
        Serial.println("BACKWARDS");
      }  

      // is moving backwards    
      leftMotorReverseSpeed   = (throttleValue * leftMotorAdjust);
      leftMotorForwardSpeed   = 0; 
    
      rightMotorReverseSpeed  = (throttleValue * rightMotorAdjust);
      rightMotorForwardSpeed  = 0; 

   
    } else {

      if(DebugMode){
        Serial.println("FORWARD");
      }  
      
      // is moving forward
      leftMotorForwardSpeed   = (throttleValue * leftMotorAdjust);
      leftMotorReverseSpeed   = 0; 
    
      rightMotorForwardSpeed  = (throttleValue * rightMotorAdjust);
      rightMotorReverseSpeed  = 0; 
    }
  
  } else { 


    if(DebugMode){
      Serial.println("************");
      Serial.println("************");
      Serial.println("************");
      Serial.println("*** IS NOT MOVING *** ");
      Serial.println("************");
      Serial.println("************");
      Serial.println("************");
    }  
    
    // Stop Jeep (gently)
    /*-------------------------------------------------------------*/
        
    // zero speed values 
    leftMotorForwardSpeed = 0;
    leftMotorReverseSpeed = 0; 
    rightMotorForwardSpeed = 0;
    rightMotorReverseSpeed = 0; 
    
  
  } // end IsMoving IF


  // Move Jeep
  /*-------------------------------------------------------------*/

  if(SafteyPass){
  
    // enable motors 
    digitalWrite(leftMotorForwardOutputPin, HIGH); 
    digitalWrite(leftMotorReverseOutputPin, HIGH);   
    digitalWrite(rightMotorForwardOutputPin, HIGH); 
    digitalWrite(rightMotorReverseOutputPin, HIGH);          
  } else {
    
    // disable motors 
    digitalWrite(leftMotorForwardOutputPin, LOW); 
    digitalWrite(leftMotorReverseOutputPin, LOW);   
    digitalWrite(rightMotorForwardOutputPin, LOW); 
    digitalWrite(rightMotorReverseOutputPin, LOW);          
  }

    
  // apply speed
  analogWrite(leftMotorForwardSpeedPin, leftMotorForwardSpeed);
  analogWrite(leftMotorReverseSpeedPin, leftMotorReverseSpeed);
  analogWrite(rightMotorForwardSpeedPin, rightMotorForwardSpeed);
  analogWrite(rightMotorReverseSpeedPin, rightMotorReverseSpeed);

  
}// loop 
