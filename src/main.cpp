#include <Arduino.h>
#include <test.h>

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

#define USIDELEFT_TRIGGER_PIN 52
#define USIDELEFT_ECHO_PIN 53

#define USIDERIGHT_TRIGGER_PIN 46
#define USIDERIGHT_ECHO_PIN 47

#define MAZE_WIDTH 114
#define ROBOT_WIDTH 26

#define TOO_CLOSE_THRESHOLD 15  // distance in cm

HBridgeMotor topLeft(enA_leftFront, in1_leftFront, in2_leftFront);
HBridgeMotor topRight(enA_rightFront, in1_rightFront, in2_rightFront);
HBridgeMotor backLeft(enA_leftBack, in1_leftBack, in2_leftBack);
HBridgeMotor backRight(enA_rightBack, in1_rightBack, in2_rightBack);

Ultrasonic frontLeftUltrasonic(UFRONTLEFT_TRIGGER_PIN, UFRONTLEFT_ECHO_PIN);
Ultrasonic frontRightUltrasonic(UFRONTRIGHT_TRIGGER_PIN, UFRONTRIGHT_ECHO_PIN);
Ultrasonic sideLeftUltrasonic(USIDELEFT_TRIGGER_PIN, USIDELEFT_ECHO_PIN);
Ultrasonic sideRightUltrasonic(USIDERIGHT_TRIGGER_PIN, USIDERIGHT_ECHO_PIN);

// Define Forward and Stop


bool conditionLR() {
  long left = sideLeftUltrasonic.readDistance();
  long right = sideRightUltrasonic.readDistance();
  return (left + right) > (MAZE_WIDTH - ROBOT_WIDTH);
}

void driveForwardUntilFrontTooClose() {
  while (frontLeftUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD &&
         frontRightUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD &&
         !conditionLR()) {
    DriveForward(topLeft, topRight, backLeft, backRight);
  }
  StopMotors(topLeft, topRight, backLeft, backRight);
}

void driveLeftUntilCondition() {
  while (sideLeftUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD &&
         sideRightUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD) {
    StrafeLeft(topLeft, topRight, backLeft, backRight);
  }
  StopMotors(topLeft, topRight, backLeft, backRight);
}

void driveRightUntilCondition() {
  while (sideRightUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD &&
         sideLeftUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD) {
    StrafeRight(topLeft, topRight, backLeft, backRight);
  }
  StopMotors(topLeft, topRight, backLeft, backRight);
}

void setup() {
  backLeft.setMotorSpeed(200);
  backRight.setMotorSpeed(200);
  topLeft.setMotorSpeed(200);
  topRight.setMotorSpeed(200);
}

void loop() {
  driveForwardUntilFrontTooClose();
  driveLeftUntilCondition();
  driveForwardUntilFrontTooClose();
  driveRightUntilCondition();
}
