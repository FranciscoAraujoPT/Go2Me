#include <SoftwareSerial.h>

SoftwareSerial bluetooth(10, 11); // RX, TX

// Pin definitions for motor control (direction and PWM)
const int motorFLDir = 2;  // Front-left motor direction pin
const int motorFLPWM = 3;  // Front-left motor PWM pin

const int motorFRDir = 4;  // Front-right motor direction pin
const int motorFRPWM = 5;  // Front-right motor PWM pin

const int motorRLDir = 6;  // Rear-left motor direction pin
const int motorRLPWM = 7;  // Rear-left motor PWM pin

const int motorRRDir = 8;  // Rear-right motor direction pin
const int motorRRPWM = 9;  // Rear-right motor PWM pin

const int MID_POINT = 128;  // Middle value for the steering wheel

int forwardPedal = 0;
int backwardPedal = 0;
int steeringDiff = 0;

String inputSentence = "";

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  
  Serial.println("Waiting for Bluetooth data...");
  
  // Set up motor pins as outputs
  pinMode(motorFLDir, OUTPUT);
  pinMode(motorFLPWM, OUTPUT);
  pinMode(motorFRDir, OUTPUT);
  pinMode(motorFRPWM, OUTPUT);
  pinMode(motorRLDir, OUTPUT);
  pinMode(motorRLPWM, OUTPUT);
  pinMode(motorRRDir, OUTPUT);
  pinMode(motorRRPWM, OUTPUT);
}

void updateSpeed(int throttleValue) {
  if (steeringDiff > 0) {
      analogWrite(motorFLPWM, throttleValue);  // Front-left motor (PWM)
      analogWrite(motorFRPWM, throttleValue - steeringDiff); // Front-right motor (PWM)
      analogWrite(motorRLPWM, throttleValue);   // Rear-left motor (PWM)
      analogWrite(motorRRPWM, throttleValue - steeringDiff);  // Rear-right motor (PWM)
  } else if (steeringDiff < 0) {
      analogWrite(motorFLPWM, throttleValue + steeringDiff);  // Front-left motor (PWM)
      analogWrite(motorFRPWM, throttleValue); // Front-right motor (PWM)
      analogWrite(motorRLPWM, throttleValue + steeringDiff);   // Rear-left motor (PWM)
      analogWrite(motorRRPWM, throttleValue);  // Rear-right motor (PWM)
  } else {
      analogWrite(motorFLPWM, throttleValue);  // Front-left motor (PWM)
      analogWrite(motorFRPWM, throttleValue); // Front-right motor (PWM)
      analogWrite(motorRLPWM, throttleValue);   // Rear-left motor (PWM)
      analogWrite(motorRRPWM, throttleValue);  // Rear-right motor (PWM)
  }
}

void setMotorDirectionForward() {
  // Set motors to move forward
  digitalWrite(motorFLDir, LOW);
  digitalWrite(motorFRDir, LOW);
  digitalWrite(motorRLDir, LOW);
  digitalWrite(motorRRDir, LOW);
}

void setMotorDirectionBackward() {
  // Set motors to move backward
  digitalWrite(motorFLDir, HIGH);
  digitalWrite(motorFRDir, HIGH);
  digitalWrite(motorRLDir, HIGH);
  digitalWrite(motorRRDir, HIGH);
}

void stopMotors() {
  // Stop the motors by setting PWM to 0
  analogWrite(motorFLPWM, 0);
  analogWrite(motorFRPWM, 0);
  analogWrite(motorRLPWM, 0);
  analogWrite(motorRRPWM, 0);
}

void loop() {
  while (bluetooth.available()) {
    char receivedChar = bluetooth.read();
    inputSentence += receivedChar;  
    if (receivedChar == '\n') {
      Serial.print("Received full sentence: ");
      Serial.println(inputSentence);
      int auxCounter = 0;
      while (true){
        char c = inputSentence[auxCounter];
        if(c == 'Z') {
            backwardPedal = 255 - inputSentence.substring(auxCounter+1).toInt();
            Serial.print("Motor Backward: ");
            Serial.println(backwardPedal);
        } else if(c == 'Y') {
            forwardPedal = 255 -inputSentence.substring(auxCounter+1).toInt(); // Pedal go from 255 to 0 
            Serial.print("Motor Forward: ");
            Serial.println(forwardPedal); 
            break;
        } else if(c == 'X') {
            steeringDiff = inputSentence.substring(auxCounter+1).toInt() - MID_POINT;
            Serial.print("Sterring: ");
            Serial.println(steeringDiff);
            break;
        }
        auxCounter++;
      }
      if (forwardPedal > 0 && backwardPedal < 10) {
        updateSpeed(forwardPedal);
        setMotorDirectionForward();
      } else if (backwardPedal > 0 && forwardPedal < 10) {
        updateSpeed(backwardPedal);
        setMotorDirectionBackward();
      } else {
        backwardPedal = 0;
        forwardPedal = 0;
        stopMotors();
        return;
      }
      inputSentence = "";
    }
    
  }
}
