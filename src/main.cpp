#include <Arduino.h>
#include <test.h>


// left front
#define enA_leftFront 11
#define in2_leftFront 22
#define in1_leftFront 23

// right motor
#define enA_leftBack 12
#define in1_leftBack 25
#define in2_leftBack 24

// right front
#define enA_rightFront 10
#define in1_rightFront 48
#define in2_rightFront 50

//right back
#define enA_rightBack 8
#define in1_rightBack 49
#define in2_rightBack 51


HBridgeMotor topLeft(enA_leftFront, in1_leftFront, in2_leftFront);
HBridgeMotor topRight(enA_rightFront, in1_rightFront, in2_rightFront);
HBridgeMotor backLeft(enA_leftBack, in1_leftBack, in2_leftBack);
HBridgeMotor backRight(enA_rightBack, in1_rightBack, in2_rightBack);

void setup() 
{
  backRight.setMotorSpeed(BR_SPEED); // 170 - 230
  backLeft.setMotorSpeed(BL_SPEED);
  topLeft.setMotorSpeed(TL_SPEED);
  topRight.setMotorSpeed(TR_SPEED);

  DriveForward(topLeft, topRight, backLeft, backRight);
  delay(5000);
  StrafeLeft(topLeft, topRight, backLeft, backRight);
  delay(5000);
  StrafeRight(topLeft, topRight, backLeft, backRight);
  delay(5000);
  DriveForward(topLeft, topRight, backLeft, backRight);
}

void loop()
{

    
}


