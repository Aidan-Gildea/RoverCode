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

// Ultrasonic sensors
#define UFRONTLEFT_TRIGGER_PIN 43
#define UFRONTLEFT_ECHO_PIN 42

#define UFRONTRIGHT_TRIGGER_PIN 44
#define UFRONTRIGHT_ECHO_PIN 45

#define USIDELEFT_TRIGGER_PIN 46
#define USIDELEFT_ECHO_PIN 47

#define USIDERIGHT_TRIGGER_PIN 52
#define USIDERIGHT_ECHO_PIN 53

#define MAZE_WIDTH 114 // Example width of the maze in cm
#define ROBOT_WIDTH 26 // Example width of the robot in cm


HBridgeMotor topLeft(enA_leftFront, in1_leftFront, in2_leftFront);
HBridgeMotor topRight(enA_rightFront, in1_rightFront, in2_rightFront);
HBridgeMotor backLeft(enA_leftBack, in1_leftBack, in2_leftBack);
HBridgeMotor backRight(enA_rightBack, in1_rightBack, in2_rightBack);


Ultrasonic frontLeftUltrasonic(UFRONTLEFT_TRIGGER_PIN, UFRONTLEFT_ECHO_PIN);
Ultrasonic frontRightUltrasonic(UFRONTRIGHT_TRIGGER_PIN, UFRONTRIGHT_ECHO_PIN);
Ultrasonic sideLeftUltrasonic(USIDELEFT_TRIGGER_PIN, USIDELEFT_ECHO_PIN);
Ultrasonic sideRightUltrasonic(USIDERIGHT_TRIGGER_PIN, USIDERIGHT_ECHO_PIN);

bool conditionLR()
{
    long left = sideLeftUltrasonic.readDistance();
    long right = sideRightUltrasonic.readDistance();
    return (left + right) > (MAZE_WIDTH - ROBOT_WIDTH);
}

void setup() 
{
  backRight.setMotorSpeed(BR_SPEED); // 170 - 230
  backLeft.setMotorSpeed(BL_SPEED);
  topLeft.setMotorSpeed(TL_SPEED);
  topRight.setMotorSpeed(TR_SPEED);
  
}

void loop()
{

}

    


