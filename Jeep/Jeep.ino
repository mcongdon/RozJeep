

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
int throttleMin     = 0; 
int throttleMax     = 1023; 

void setup() {
  
  Serial.println("Setup Started:");
    
  //Init pins 
  Serial.println("Inititalizing Pins...");
  pinMode(leftMotorPin, OUTPUT);
  pinMode(rightMotorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Initialize and calibrate sensors. 
  Serial.println("Calibrating Sensors...");
  calibrateThrottle(); 

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

  // adjust for direction sensor
  // TODO:
  
  // output to motors
  analogWrite(leftMotorPin, throttleValue);
  analogWrite(rightMotorPin, throttleValue);

  Serial.println("Output Left Motor: " + throttleValue);
  Serial.println("Output Right Motor: " + throttleValue);
  
}


void calibrateThrottle(){
  
  // calibrate min voltage of throttle pedal
  Serial.println("Throttle Calibration Started");
  
  while (millis() < 1000) {
    throttleValue = analogRead(throttlePin);

    // record the minimum sensor value
    if (throttleValue < throttleMin) {
      throttleMin = throttleValue;
    }
    
    Serial.println("Throttle Minimum Val: " + throttleMin);
  }

  Serial.println("Throttle Calibration Ended:");
}
