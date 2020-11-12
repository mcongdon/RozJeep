/*
 * Engine control unit for Power Wheels Escalade Dual Motor aka Jeep
 *
 * Author: Michael Congdon
 *
 */

#include <TimerOne.h>

// pin definitions
int throttlePin   = A0;     // input pin for throttle pedal
int leftMotorPin  = 10;     // pwm output left motor
int rightMotorPin = 11;     // pwm output right motor
int ledPin        = 13;     // LED on Arduino Board

// throttle calibration
int throttleValue   = 0;
int throttleMin     = 100;
int throttleMax     = 860;
int throttleOutR    = 0; 
int throttleOutL    = 0; 



void setup() {
  
  // open serial port connection
  Serial.begin(9600); 
   
  //Init pins
  pinMode(leftMotorPin, OUTPUT);
  pinMode(rightMotorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // calibrate min voltage of throttle pedal
  throttleValue = analogRead(throttlePin);
  throttleMin = throttleValue + 10; 

  // init pwm timer
  Timer1.initialize(8);// Frequency, 8us = 125KHz
  
  // init motors 
  Timer1.pwm(leftMotorPin, throttleMin);
  Timer1.pwm(rightMotorPin, throttleMin);

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

  // adjust for direction sensor
  throttleOutL = throttleValue; 
  throttleOutR = throttleValue; 

  // output to motors
  Timer1.pwm(leftMotorPin, throttleOutL);
  Timer1.pwm(rightMotorPin, throttleOutR);
  
  //delay(2);
  
  Serial.println(throttleOutL);
  Serial.println(throttleOutR);
 
}
