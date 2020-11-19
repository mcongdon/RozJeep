/*
 * Engine control unit for Power Wheels Escalade Dual Motor aka Jeep
 *
 * Author: Michael Congdon
 *
 */

// pin definitions
int throttleInputPin   = A0;     // input pin for throttle pedal
int pingInputPin       = 12;     // input pin for front ping (collision detection)


int leftMotorForwardIndicatorPin = 22;      // Left Motor is going forward indicator
int leftMotorReverseIndicatorPin = 23;      // Left Motor is going reverse indicator
int leftMotorForwardOutputPin    = 24;      // Tell Left Motor to go forward pin
int leftMotorReverseOutputPin    = 25;      // Tell Left Motor to go reverse pin
int leftMotorForwardSpeedPin     = 5;       // pwm output left motor Forward Speed
int leftMotorReverseSpeedPin     = 6;       // pwm output left motor Reverse

int rightMotorForwardIndicatorPin = 26;      // Right Motor is going forward indicator
int rightMotorReverseIndicatorPin = 27;      // Right Motor is going reverse indicator
int rightMotorForwardOutputPin    = 28;      // Tell Right Motor to go forward pin
int rightMotorReverseOutputPin    = 29;      // Tell Right Motor to go reverse pin
int rightMotorForwardSpeedPin     = 9;       // pwm output right motor Forward Speed
int rightMotorReverseSpeedPin     = 10;       // pwm output right motor Reverse


int ledPin        = 13;     // LED on Arduino Board

// throttle calibration
int throttleValue   = 0;
int throttleMin     = 100;
int throttleMax     = 800;

int leftMotorForwardSpeed     = 0; 
int leftMotorReverseSpeed     = 0; 
int rightMotorForwardSpeed    = 0; 
int rightMotorReverseSpeed    = 0; 

// initial state stopped
bool MoveForward = false; 

void setup() {
  
  // open serial port connection
  Serial.begin(9600); 
   
  //Init sensor pins
  pinMode(throttleInputPin, INPUT);
  pinMode(pingInputPin, INPUT);
  
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
    
}

void loop() {

  
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  long pingDuration, pingInches;
  pinMode(pingInputPin, OUTPUT);
  digitalWrite(pingInputPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingInputPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingInputPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(pingInputPin, INPUT);
  pingDuration = pulseIn(pingInputPin, HIGH);

  // convert the time into a distance
  pingInches = microsecondsToInches(pingDuration);

  Serial.println(inches); 

  
  // read throttle pedal
  throttleValue = analogRead(throttleInputPin);
  // apply the calibration to the sensor reading
  throttleValue = map(throttleValue, throttleMin, throttleMax, 0, 255);
  // in case the sensor value is outside the range seen during calibration
  throttleValue = constrain(throttleValue, 0, 255);


  //Resolve direction and collision sensors 
  // Need to handle reverse... 
  if(throttleValue > 80 && pingInches > 2) 
  {
    MoveForward = true; 
  } else {
    
    MoveForward = false; 
  }
  
  
  // Apply direction var to motor control
  if(MoveForward) {
    
    // set forward speed
    leftMotorForwardSpeed   = throttleValue;
    leftMotorReverseSpeed   = 0; 
    
    rightMotorForwardSpeed  = throttleValue;
    rightMotorReverseSpeed  = 0; 

    // enable motors
    digitalWrite(leftMotorForwardOutputPin, HIGH); 
    digitalWrite(leftMotorReverseOutputPin, HIGH);   
    digitalWrite(rightMotorForwardOutputPin, HIGH); 
    digitalWrite(rightMotorReverseOutputPin, HIGH);   
    
  } else {
    // apply brakes 
    leftMotorForwardSpeed = 0;
    leftMotorReverseSpeed = 0; 
    rightMotorForwardSpeed = 0;
    rightMotorReverseSpeed = 0; 
    
    // shut off motors
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
    
}


long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are 73.746
  // microseconds per inch (i.e. sound travels at 1130 feet per second).
  // This gives the distance travelled by the ping, outbound and return,
  // so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}
