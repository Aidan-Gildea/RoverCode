#include <Arduino.h>
#include <test.h>
#include "Servo.h"

// hello


// Motor pin definitions
#define enA_leftFront 2
#define in2_leftFront 22
#define in1_leftFront 23

#define enA_leftBack 3
#define in1_leftBack 25
#define in2_leftBack 24

#define enA_rightFront 5
#define in1_rightFront 27
#define in2_rightFront 26

#define enA_rightBack 4
#define in1_rightBack 29
#define in2_rightBack 28

// Ultrasonic sensors
#define UFRONTLEFT_TRIGGER_PIN 50
#define UFRONTLEFT_ECHO_PIN 51

#define UFRONTRIGHT_TRIGGER_PIN 48
#define UFRONTRIGHT_ECHO_PIN 49

#define USIDELEFT_TRIGGER_PIN 40
#define USIDELEFT_ECHO_PIN 41

#define USIDERIGHT_TRIGGER_PIN 46
#define USIDERIGHT_ECHO_PIN 47

#define MAZE_WIDTH 114
#define ROBOT_WIDTH 26

#define TOO_CLOSE_THRESHOLD 15
#define TOO_CLOSE_THRESHOLD_OFFSET 15

#define FRONT_SERVO_PIN 7
#define BACK_SERVO_PIN 8

HBridgeMotor topLeft(enA_leftFront, in1_leftFront, in2_leftFront);
HBridgeMotor topRight(enA_rightFront, in1_rightFront, in2_rightFront);
HBridgeMotor backLeft(enA_leftBack, in1_leftBack, in2_leftBack);
HBridgeMotor backRight(enA_rightBack, in1_rightBack, in2_rightBack);

Ultrasonic frontLeftUltrasonic(UFRONTLEFT_TRIGGER_PIN, UFRONTLEFT_ECHO_PIN);
Ultrasonic frontRightUltrasonic(UFRONTRIGHT_TRIGGER_PIN, UFRONTRIGHT_ECHO_PIN);
Ultrasonic sideLeftUltrasonic(USIDELEFT_TRIGGER_PIN, USIDELEFT_ECHO_PIN);
Ultrasonic sideRightUltrasonic(USIDERIGHT_TRIGGER_PIN, USIDERIGHT_ECHO_PIN);

Servo frontServo; 
Servo backServo; 

// 判斷左右側總空間是否大於迷宮寬度 - 機器寬度
bool conditionLR() {
  long left = sideLeftUltrasonic.readDistance();
  long right = sideRightUltrasonic.readDistance();
  return (left + right) > (MAZE_WIDTH - ROBOT_WIDTH);
}

// 前進直到前方或左右側距離過近
// drive forward works

void driveForwardUntilFrontTooClose() {
  while (
  (frontLeftUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD) &&
  frontRightUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD)
  {
    DriveForward(topLeft, topRight, backLeft, backRight);
    delay(50);
  }
  StopMotors(topLeft, topRight, backLeft, backRight);
}
// 向左平移直到右側過近或前右有空間

// 
void driveLeftWhileCondition() {
  while (
    sideLeftUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD &&
    frontRightUltrasonic.readDistance() < (TOO_CLOSE_THRESHOLD + TOO_CLOSE_THRESHOLD_OFFSET)
  ) {
    StrafeLeft(topLeft, topRight, backLeft, backRight);
    delay(50);
  }
  if (frontRightUltrasonic.readDistance() > (TOO_CLOSE_THRESHOLD + TOO_CLOSE_THRESHOLD_OFFSET)) {
    delay(500);
  }
  StopMotors(topLeft, topRight, backLeft, backRight);
}

// 向右平移直到左側過近或前左有空間
void driveRightWhileCondition() {
  while (
    sideRightUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD &&
    frontLeftUltrasonic.readDistance() < (TOO_CLOSE_THRESHOLD + TOO_CLOSE_THRESHOLD_OFFSET)
  ) 
  {
    StrafeRight(topLeft, topRight, backLeft, backRight);
    delay(50);
  }
  if (frontLeftUltrasonic.readDistance() > (TOO_CLOSE_THRESHOLD + TOO_CLOSE_THRESHOLD_OFFSET)) {
    delay(500);
  }
  StopMotors(topLeft, topRight, backLeft, backRight);
}

void SetArmPosition(int angle)
{
  if(angle < 90 || angle > 180) return;
  frontServo.write(angle);
  backServo.write(angle);
  delay(300); // Allow time for servo to reach position
}
void setup() {
  Serial.begin(9600);

  backLeft.setMotorSpeed(BL_SPEED);
  backRight.setMotorSpeed(BR_SPEED);
  topLeft.setMotorSpeed(TL_SPEED);
  topRight.setMotorSpeed(TR_SPEED);

  StopMotors(topLeft, topRight, backLeft, backRight);

  frontServo.attach(FRONT_SERVO_PIN);
  backServo.attach(BACK_SERVO_PIN);

  SetArmPosition(90); // Set initial position to 90 degrees

}

bool TestUltrasonic(Ultrasonic& sensor, int flagDistance) {

  long distance = sensor.readDistance();
  if(distance < flagDistance) {
    return true;
  }
  return false;

}

void loop() {
  driveForwardUntilFrontTooClose();
  delay(300);
  driveLeftWhileCondition();
  delay(300);
  driveForwardUntilFrontTooClose();
  delay(300);
  driveRightWhileCondition();
  delay(300);
}