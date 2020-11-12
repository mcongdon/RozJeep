/*
 * Engine control unit for Power Wheels Escalade Dual Motor aka Jeep
 *
 * Author: Michael Congdon
 *
 */
 

// pin definitions
int throttlePin   = A0;     // input pin for throttle pedal
int leftMotorPin  = 10;     // pwm output left motor
int rightMotorPin = 11;     // pwm output right motor
int ledPin        = 13;     // LED on Arduino Board

// throttle calibration
int throttleValue   = 0;
int throttleMin     = 100;
int throttleMax     = 860;
int throttleOutR    = 255; 
int throttleOutL    = 255; 


void setup() {
  
  // open serial port connection
  Serial.begin(9600); 

  Serial.println("Setup Started:");
   
  //Init pins
  pinMode(leftMotorPin, OUTPUT);
  pinMode(rightMotorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  // calibrate min voltage of throttle pedal
  Serial.println("Throttle Calibration");
    
  throttleValue = analogRead(throttlePin);
  throttleMin = throttleValue + 10;

  // light indicator pin when setup complete
  Serial.println("Setup Complete:");
  digitalWrite(ledPin, HIGH);
}

void loop() {

  // read throttle pedal
  throttleValue = analogRead(throttlePin);
  // apply the calibration to the sensor reading
  throttleValue = map(throttleValue, throttleMin, throttleMax, 0, 255);
  // in case the sensor value is outside the range seen during calibration
  throttleValue = constrain(throttleValue, 0, 255);
 
  // Convert to 5v voltage reading.
  //float voltage = sensorValue * (5.0 / 1023.0);

  // invert for resistance output to motor controller. 
  throttleOutR = (255 - throttleValue); 
  throttleOutL = (255 - throttleValue); 

  // adjust for direction sensor
  // TODO:
 
  // output to motors
  analogWrite(leftMotorPin, throttleOutL);
  analogWrite(rightMotorPin, throttleOutR);
  
  //delay(2);
  
  Serial.println(throttleOutL);
  Serial.println(throttleOutR);
 
}


