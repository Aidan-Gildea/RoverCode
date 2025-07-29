#include <test.h>
#include "Servo.h"
#include <Wire.h>
#include <MPU6050_light.h> 


// hello


// todo:
// Make maze navigation faster --done
// Fill out condition L+R, The distance is 180 --done
// Only activate L+R after a certain amount of time. --done


// Get closer to right side by strafing right until certain distance is seen
// implement gyroscope



// Motor pin definitions


// i made a change
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


#define MAZE_WIDTH 140
#define ROBOT_WIDTH 20


#define TOO_CLOSE_THRESHOLD 18
#define TOO_CLOSE_THRESHOLD_OFFSET 13


#define FRONT_SERVO_PIN 7
#define BACK_SERVO_PIN 8


#define DELAY_TIME 50


#define MAZE_TIME 13500 // 7 seconds


#define STEPOVER_DELAY 200


#define TIMER_FLAG_PIN 35
#define LR_FLAG_PIN 34


#define LR_OFFSET 20
#define RIGHT_DISTANCE_TO_STOP 40
#define LEFT_DISTANCE_TO_STOP 35
#define FRONT_DISTANCE_TO_STOP 35


#define ANGLE_CORRECTION_THRESHOLD 6

#define STOP_AND_GRAB_DISTANCE 10
#define WAIT_TIME 300

#define OBJECT_DISTANCE (RIGHT_DISTANCE_TO_STOP-20) // where 2 is the offset

#define CLOSE_OBJECT 5

#define BACKWARD_TIME 300



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


LED LR_FLAG_LED(LR_FLAG_PIN);
LED TIMER_FLAG_LED(TIMER_FLAG_PIN);

MPU6050 mpu(Wire);

enum CurrentState
{
  MAZENAVIGATION,
  OBJECTDETECTION,
  DONE
};

CurrentState currentState = MAZENAVIGATION;

void SetArmPosition(int angle)
{
  if(angle < 90 || angle > 180) return;
  frontServo.write(angle);
  backServo.write(angle);
  delay(300); // Allow time for servo to reach position
}

void SetSpeeds(byte tl, byte tr, byte bl, byte br)
{
  topLeft.setMotorSpeed(tl);
  topRight.setMotorSpeed(tr);
  backLeft.setMotorSpeed(bl);
  backRight.setMotorSpeed(br);
}

// 判斷左右側總空間是否大於迷宮寬度 - 機器寬度
bool conditionLR() {
  long left = sideLeftUltrasonic.readDistance();
  long right = sideRightUltrasonic.readDistance();
  if(millis() < MAZE_TIME) return false; // Only check after MAZE_TIME has passed
  TIMER_FLAG_LED.on();
  bool success= (left + right) > (MAZE_WIDTH - ROBOT_WIDTH-LR_OFFSET);
  if(success) 
  {
    LR_FLAG_LED.on();
    currentState = OBJECTDETECTION;
    delay(300);
   // DriveBackward(topLeft, topRight, backLeft, backRight);
    //delay(400);
    SetSpeeds(SLOW_TL_FB,SLOW_TR_FB,SLOW_BL_FB, SLOW_BR_FB);
    StopMotors(topLeft, topRight, backLeft, backRight);
  }
  return success;
}

// void CorrectAngle()
// {
//  mpu.update();
//  float angleZ = mpu.getAngleZ();
//  while(angleZ > ANGLE_CORRECTION_THRESHOLD || angleZ < -ANGLE_CORRECTION_THRESHOLD) {
//    if(angleZ > 0) {
//      SpinRight(topLeft, topRight, backLeft, backRight);
//      mpu.update();
//     angleZ = mpu.getAngleZ();
//    } else {
//      SpinLeft(topLeft, topRight, backLeft, backRight);
//      mpu.update();
//      angleZ = mpu.getAngleZ();

//    }
//    mpu.update();
//    angleZ = mpu.getAngleZ();
//  }
//  StopMotors(topLeft, topRight, backLeft, backRight);
// }
// // 前進直到前方或左右側距離過近
// // drive forward works


bool driveForwardUntilFrontTooClose() {
 bool firstCondition = false; 
 while (
 (frontLeftUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD) &&
 frontRightUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD && !conditionLR()
 )
 {
   firstCondition = true;
   //CorrectAngle();
   DriveForward(topLeft, topRight, backLeft, backRight);
    mpu.update();
    Serial.println( mpu.getAngleZ());
   delay(50);
 }
 StopMotors(topLeft, topRight, backLeft, backRight);
 return firstCondition;
}
// 向左平移直到右側過近或前右有空間


//
bool driveLeftWhileCondition() {
  bool firstCondition = false; 
  while (
    (sideLeftUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD &&
    frontRightUltrasonic.readDistance() < (TOO_CLOSE_THRESHOLD + TOO_CLOSE_THRESHOLD_OFFSET)) && !conditionLR()
 )
 {
   //CorrectAngle();
   firstCondition = true;
   StrafeLeft(topLeft, topRight, backLeft, backRight);
   delay(50); // to prevent junk values on ultrasonic sensors
    mpu.update();
    Serial.println( mpu.getAngleZ());
  }
  if (frontRightUltrasonic.readDistance() > (TOO_CLOSE_THRESHOLD + TOO_CLOSE_THRESHOLD_OFFSET)) {
    delay(STEPOVER_DELAY);
 }
 StopMotors(topLeft, topRight, backLeft, backRight);
 return firstCondition;
}



// 向右平移直到左側過近或前左有空間
bool driveRightWhileCondition() {
  bool firstCondition = false; 
  while (
    (sideRightUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD &&
    frontLeftUltrasonic.readDistance() < (TOO_CLOSE_THRESHOLD + TOO_CLOSE_THRESHOLD_OFFSET)) && !conditionLR()
  ) {
    firstCondition = true;
    //CorrectAngle();
    StrafeRight(topLeft, topRight, backLeft, backRight);
    mpu.update();
    Serial.println( mpu.getAngleZ());
    delay(50);
  }  
  if (frontLeftUltrasonic.readDistance() > (TOO_CLOSE_THRESHOLD + TOO_CLOSE_THRESHOLD_OFFSET)) {
    delay(STEPOVER_DELAY);
  }  
  StopMotors(topLeft, topRight, backLeft, backRight);
  return firstCondition;
} 


void setup() {
 Serial.begin(9600);
//  Wire.begin();

 delay(1000);
//  mpu.begin();
//  mpu.calcOffsets(true,true);

  SetSpeeds(TL_L_SPEED, TR_L_SPEED, BL_L_SPEED, BR_L_SPEED);


 StopMotors(topLeft, topRight, backLeft, backRight);


 frontServo.attach(FRONT_SERVO_PIN);
 backServo.attach(BACK_SERVO_PIN);


 SetArmPosition(90); // Set initial position to 180 degrees

}


enum OBJDState
{
  STRAFERIGHTUNTILRIGHTDISTANCE,
  DRIVEFORWARDUNTILRIGHTDISTANCEDETECTED,
  STRAFERIGHTUNTILOBJECTISCLOSE,
  PAUSEANDGRABOBJECT,
  STRAFELEFTUNTILLEFTDISTANCE,
  DETECTFRONTSENSORUNTILDISTANCE,
  RELEASEOBJECT
};


OBJDState objdState = STRAFERIGHTUNTILRIGHTDISTANCE;

void loop(){
  StrafeLeft(topLeft, topRight, backLeft, backRight);
}
// {
//   StrafeRight(topLeft, topRight, backLeft, backRight);
// }

// int count = 0; 
// void loop() {

//   if(currentState == MAZENAVIGATION) 
//   {
//     if(driveForwardUntilFrontTooClose()) delay(DELAY_TIME);
//     if(driveLeftWhileCondition()) delay(DELAY_TIME);
//     if(driveForwardUntilFrontTooClose()) delay(DELAY_TIME);
//     if(driveRightWhileCondition()) delay(DELAY_TIME);
//   }
//   else if(currentState == OBJECTDETECTION) 
//   {
//     long sideRightDistance = sideRightUltrasonic.readDistance();
//     long frontDistance = (frontRightUltrasonic.readDistance() + frontLeftUltrasonic.readDistance()) / 2;

//     if(objdState == STRAFERIGHTUNTILRIGHTDISTANCE)
//     {
//       SetSpeeds(SLOW_TL_SR, SLOW_TR_SR, SLOW_BL_SR, SLOW_BR_SR);
//       StrafeRight(topLeft, topRight, backLeft, backRight);
//       if((sideRightDistance < RIGHT_DISTANCE_TO_STOP)&& count >=3) {
//         StopMotors(topLeft, topRight, backLeft, backRight);
//         objdState = DRIVEFORWARDUNTILRIGHTDISTANCEDETECTED;
//         SetSpeeds(SLOW_TL_FB, SLOW_TR_FB, SLOW_BL_FB, SLOW_BR_FB);
//       }
//       else if((sideRightDistance < RIGHT_DISTANCE_TO_STOP)&& count < 3)
//       {
//         count++;
//       }
//       else
//       {
//         count = 0; 
//       }
//     }
//     else if(objdState == DRIVEFORWARDUNTILRIGHTDISTANCEDETECTED)
//     {
//       if((sideRightDistance < OBJECT_DISTANCE)) // 45 is an arbitrary value to say that the thing is detected
//       {
//         StopMotors(topLeft, topRight, backLeft, backRight);
//         objdState = STRAFERIGHTUNTILOBJECTISCLOSE;

//         //DriveBackward(topLeft, topRight, backLeft, backRight);
        
//         //delay(BACKWARD_TIME);
//         StopMotors(topLeft, topRight, backLeft, backRight);
//         SetSpeeds(SLOW_TL_SR, SLOW_TR_SR, SLOW_BL_SR, SLOW_BR_SR);
          
        
//       }
//       else
//       {
//         DriveForward(topLeft, topRight, backLeft, backRight);
//       }
      
//     }
//     else if(objdState == STRAFERIGHTUNTILOBJECTISCLOSE)
//     {
//       StopMotors(topLeft, topRight, backLeft, backRight);
//       StrafeRight(topLeft, topRight, backLeft, backRight);
//       if(sideRightDistance < CLOSE_OBJECT) // arbitrary value to stay that the object is close
//       {
//         objdState = PAUSEANDGRABOBJECT;
//       }
//     }
//     else if(objdState == PAUSEANDGRABOBJECT)
//     {
//       StopMotors(topLeft, topRight, backLeft, backRight);

//       SetArmPosition(0); // Grab the object
//       delay(300);
//       objdState = STRAFELEFTUNTILLEFTDISTANCE;
//       SetSpeeds(SLOW_TL_SL, SLOW_TR_SL, SLOW_BL_SL, SLOW_BR_SL);
//     }
//     else if(objdState == STRAFELEFTUNTILLEFTDISTANCE)
//     {
//       StrafeLeft(topLeft, topRight, backLeft, backRight);
//       if(sideLeftUltrasonic.readDistance() < LEFT_DISTANCE_TO_STOP) {
//         objdState = DETECTFRONTSENSORUNTILDISTANCE;
//       }
//     }
//     else if(objdState == DETECTFRONTSENSORUNTILDISTANCE)
//     {
//       if(frontDistance > FRONT_DISTANCE_TO_STOP) 
//       {
//         while(frontDistance > FRONT_DISTANCE_TO_STOP)
//         {
//           frontDistance = (frontRightUltrasonic.readDistance() + frontLeftUltrasonic.readDistance()) / 2;
//           DriveForward(topLeft, topRight, backLeft, backRight);
//         }
//         objdState = DETECTFRONTSENSORUNTILDISTANCE;
//         return;
//       } 
//       else 
//       {
//         while(frontDistance < FRONT_DISTANCE_TO_STOP)
//         {
//           frontDistance = (frontRightUltrasonic.readDistance() + frontLeftUltrasonic.readDistance()) / 2;
//           DriveBackward(topLeft, topRight, backLeft, backRight);
//         }
//         objdState = DETECTFRONTSENSORUNTILDISTANCE;
//         return;
//       }
//       objdState = RELEASEOBJECT;
//     }
//     else if(objdState == RELEASEOBJECT)
//     {
//       StopMotors(topLeft, topRight, backLeft, backRight);
//       delay(300);
//       SetArmPosition(90); // Release the object
//       delay(300);
//       objdState = STRAFERIGHTUNTILRIGHTDISTANCE; // Reset state to start over
//     }
//     delay(100);
//   }
//   else if(currentState == DONE) 
//   {
//     // twerk

//     SpinLeft(topLeft, topRight, backLeft, backRight);
//     delay(300); // Rotate for 1 second
//     SpinRight(topLeft, topRight, backLeft, backRight);
//     delay(300); // Rotate for 1 second
//   }
// }
  

