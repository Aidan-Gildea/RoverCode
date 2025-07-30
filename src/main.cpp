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



//hardcoded values based off of envionrment: 



// object detection strafe right delay time

#define level 0

#define OBJD_SR_DT 2000 // 1000ms
#define OBJD_SL_DT 4000
#define RIGHT_DISTANCE_TO_STOP 70 // if in quadrant 1, do 35, else do 60
#define OBJECT_DISTANCE 33 // if in quadrant 1, do 21

#define OBJD_DF_DT 1000

#define LR_TIME 0

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


#define TOO_CLOSE_THRESHOLD_L 18
#define TOO_CLOSE_THRESHOLD_R 20
#define TOO_CLOSE_THRESHOLD_FRONT 20


#define TOO_CLOSE_THRESHOLD_OFFSET 13


#define FRONT_SERVO_PIN 7
#define BACK_SERVO_PIN 8


#define DELAY_TIME 50


#define MAZE_TIME 10000 // 7 seconds


#define STEPOVER_DELAY 200


#define TIMER_FLAG_PIN 35
#define LR_FLAG_PIN 34
#define OBJD_TIMER_PIN 38
#define OBJECTDETECTED_PIN 52

#define LR_OFFSET 20
#define LEFT_DISTANCE_TO_STOP 15
#define FRONT_DISTANCE_TO_STOP 50


#define ANGLE_CORRECTION_THRESHOLD 6

#define STOP_AND_GRAB_DISTANCE 15
#define WAIT_TIME 300


#define CLOSE_OBJECT 10

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
LED OBJD_TIMER_LED(OBJD_TIMER_PIN);
LED OBJECTDETECTED_LED(OBJECTDETECTED_PIN);

MPU6050 mpu(Wire);

enum CurrentState
{
  MAZENAVIGATION,
  OBJECTDETECTION,
  DONE
};

CurrentState currentState = MAZENAVIGATION;

Timer newTimer(OBJD_SR_DT);

void SetArmPosition(int angle)
{
  if(angle < 0  || angle > 180) return;
  frontServo.write(angle);
  backServo.write((180 - angle));
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
    //delay(300);
   // DriveBackward(topLeft, topRight, backLeft, backRight);
    //delay(400);
    //SetSpeeds(SLOW_TL_FB,SLOW_TR_FB,SLOW_BL_FB, SLOW_BR_FB);
    delay(LR_TIME);
    StopMotors(topLeft, topRight, backLeft, backRight);

    newTimer.reset();
    newTimer.setInterval(OBJD_SR_DT);
    OBJD_TIMER_LED.off();
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


bool driveForwardUntilFrontTooClose(bool isMazeNavigation) // makes sense
{


// when ismazenav, we pass it into the driveforward function which will take the 
// boolean and set the motors to the corresponding speed based off of the current
// state of our robot. if in mazenav, then we send true and it will set all speeds
// to fast, otherwise it will set all speeds to slow. 


 bool firstCondition = false; 
 while (
 (frontLeftUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD_FRONT) &&
 frontRightUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD_FRONT && !conditionLR()
 )
 {
   firstCondition = true;
   //CorrectAngle();
   DriveForward(topLeft, topRight, backLeft, backRight, isMazeNavigation);
    mpu.update();
    Serial.println( mpu.getAngleZ());
   delay(50);
 }
 StopMotors(topLeft, topRight, backLeft, backRight);
 return firstCondition;
}
// 向左平移直到右側過近或前右有空間



//
bool driveLeftWhileCondition(bool isMazeNavigation) { // seems logically correct
  if(isMazeNavigation)
  {
    SetSpeeds(TL_L_SPEED, TR_L_SPEED, BL_L_SPEED, BR_L_SPEED);
  }
  else
  {
    SetSpeeds(SLOW_TL_SL, SLOW_TR_SL, SLOW_BL_SL, SLOW_BR_SL);
  }

  bool firstCondition = false; 
  while (
    (sideLeftUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD_L &&
    frontRightUltrasonic.readDistance() < (TOO_CLOSE_THRESHOLD_FRONT + TOO_CLOSE_THRESHOLD_OFFSET)) && !conditionLR()
 )
 {
   //CorrectAngle();
   firstCondition = true;
   StrafeLeft(topLeft, topRight, backLeft, backRight, isMazeNavigation);
   delay(50); // to prevent junk values on ultrasonic sensors
    mpu.update();
    Serial.println( mpu.getAngleZ());
  }
  if (frontRightUltrasonic.readDistance() > (TOO_CLOSE_THRESHOLD_FRONT + TOO_CLOSE_THRESHOLD_OFFSET)) {
    delay(STEPOVER_DELAY);
 }
 StopMotors(topLeft, topRight, backLeft, backRight);
 return firstCondition;
}



// 向右平移直到左側過近或前左有空間
bool driveRightWhileCondition(bool isMazeNavigation) {  // seems logically correct
  if(isMazeNavigation)
  {
    SetSpeeds(TL_R_SPEED, TR_R_SPEED, BL_R_SPEED, BR_R_SPEED);
  }
  else
  {
    SetSpeeds(SLOW_TL_SR, SLOW_TR_SR, SLOW_BL_SR, SLOW_BR_SR);
  }

  bool firstCondition = false; 
  while (
    (sideRightUltrasonic.readDistance() > TOO_CLOSE_THRESHOLD_R &&
    frontLeftUltrasonic.readDistance() < (TOO_CLOSE_THRESHOLD_FRONT + TOO_CLOSE_THRESHOLD_OFFSET)) && !conditionLR()
  ) {
    firstCondition = true;
    //CorrectAngle();
    StrafeRight(topLeft, topRight, backLeft, backRight, isMazeNavigation);
    mpu.update();
    Serial.println( mpu.getAngleZ());
    delay(50);
  }  
  if (frontLeftUltrasonic.readDistance() > (TOO_CLOSE_THRESHOLD_FRONT + TOO_CLOSE_THRESHOLD_OFFSET)) {
    delay(STEPOVER_DELAY);
  }  
  StopMotors(topLeft, topRight, backLeft, backRight);
  return firstCondition;
} 


void setup() {
 Serial.begin(9600);
//  Wire.begin();
//  mpu.begin();
//  mpu.calcOffsets(true,true);

  SetSpeeds(TL_L_SPEED, TR_L_SPEED, BL_L_SPEED, BR_L_SPEED);


 StopMotors(topLeft, topRight, backLeft, backRight);


 frontServo.attach(FRONT_SERVO_PIN);
 backServo.attach(BACK_SERVO_PIN);


 SetArmPosition(90); // Set initial position to 180 degrees

 OBJD_TIMER_LED.on();

 OBJECTDETECTED_LED.off();

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


int count = 0; 

bool dfBool = false;


void loop() {

  // maze naviation 
  if(currentState == MAZENAVIGATION) 
  {
    if(driveForwardUntilFrontTooClose(true)) delay(DELAY_TIME);
    if(driveLeftWhileCondition(true)) delay(DELAY_TIME);
    if(driveForwardUntilFrontTooClose(true)) delay(DELAY_TIME);
    if(driveRightWhileCondition(true)) delay(DELAY_TIME);
  }

  // object detection 



  else if(currentState == OBJECTDETECTION) 
  {
    long sideRightDistance = sideRightUltrasonic.readDistance();
    long frontDistance = (frontRightUltrasonic.readDistance() + frontLeftUltrasonic.readDistance()) / 2;

    if(objdState == STRAFERIGHTUNTILRIGHTDISTANCE)
    {
      //if(newTimer.isReady()) OBJD_TIMER_LED.on();
      
      StrafeRight(topLeft, topRight, backLeft, backRight, false);

      if(((sideRightDistance < RIGHT_DISTANCE_TO_STOP)&& count >=3) && newTimer.isReady()) {
        StopMotors(topLeft, topRight, backLeft, backRight);
        objdState = DRIVEFORWARDUNTILRIGHTDISTANCEDETECTED;



        // implement a timer here for level 1 
      }
      else if((sideRightDistance < RIGHT_DISTANCE_TO_STOP)&& count < 3)
      {
        count++;
      }
      else
      {
        count = 0; 
      }
    }

    //---------------------------------------------
    else if(objdState == DRIVEFORWARDUNTILRIGHTDISTANCEDETECTED)
    {
      if((sideRightDistance < OBJECT_DISTANCE)) // 45 is an arbitrary value to say that the thing is detected
      {
        OBJECTDETECTED_LED.on();
        StopMotors(topLeft, topRight, backLeft, backRight);
        objdState = STRAFERIGHTUNTILOBJECTISCLOSE;
        //if(dfBool == true || level == 0)
        //{


          //DriveBackward(topLeft, topRight, backLeft, backRight);
          
          //delay(BACKWARD_TIME);
          //StopMotors(topLeft, topRight, backLeft, backRight);
          //SetSpeeds(SLOW_TL_SR, SLOW_TR_SR, SLOW_BL_SR, SLOW_BR_SR);
            
        // }
        // else
        // {
        //   StopMotors(topLeft, topRight, backLeft, backRight);
        //   delay(500);
        //   DriveForward(topLeft, topRight, backLeft, backRight, false);
        //   delay(OBJD_DF_DT);
        //   dfBool = true;
        // }
        
      }
      else
      {
        DriveForward(topLeft, topRight, backLeft, backRight, false);
      }
      
    }
    //---------------------------------------------
    else if(objdState == STRAFERIGHTUNTILOBJECTISCLOSE)
    {
      StopMotors(topLeft, topRight, backLeft, backRight);
      StrafeRight(topLeft, topRight, backLeft, backRight, false);
      if(sideRightDistance < CLOSE_OBJECT) // arbitrary value to stay that the object is close
      {
        objdState = PAUSEANDGRABOBJECT;
      }
    }
    else if(objdState == PAUSEANDGRABOBJECT)
    {
      StopMotors(topLeft, topRight, backLeft, backRight);

      SetArmPosition(0); // Grab the object
      delay(300);
      objdState = STRAFELEFTUNTILLEFTDISTANCE;
      //SetSpeeds(SLOW_TL_SL, SLOW_TR_SL, SLOW_BL_SL, SLOW_BR_SL);

      OBJD_TIMER_LED.off();
      newTimer.reset();
      newTimer.setInterval(OBJD_SL_DT);
    }
    else if(objdState == STRAFELEFTUNTILLEFTDISTANCE)
    {
      if(newTimer.isReady()) OBJD_TIMER_LED.on();

      StrafeLeft(topLeft, topRight, backLeft, backRight, false);
      if(sideLeftUltrasonic.readDistance() < LEFT_DISTANCE_TO_STOP && newTimer.isReady()) {
        objdState = DETECTFRONTSENSORUNTILDISTANCE;
      }
    }
    else if(objdState == DETECTFRONTSENSORUNTILDISTANCE)
    {
      if(frontDistance > FRONT_DISTANCE_TO_STOP) 
      {
        while(frontDistance > FRONT_DISTANCE_TO_STOP)
        {
          frontDistance = (frontRightUltrasonic.readDistance() + frontLeftUltrasonic.readDistance()) / 2;
          DriveForward(topLeft, topRight, backLeft, backRight, false);
        }
        objdState = RELEASEOBJECT;
        return;
      } 
      else 
      {
        while(frontDistance < FRONT_DISTANCE_TO_STOP)
        {
          frontDistance = (frontRightUltrasonic.readDistance() + frontLeftUltrasonic.readDistance()) / 2;
          DriveBackward(topLeft, topRight, backLeft, backRight, false);
        }
        objdState = RELEASEOBJECT;
        return;
      }
      objdState = RELEASEOBJECT;
    }
    else if(objdState == RELEASEOBJECT)
    {
      StopMotors(topLeft, topRight, backLeft, backRight);
      //delay(300);
      //SetArmPosition(90); // Release the object
      //delay(300);
      currentState = DONE; // Reset state to start over
    }
    delay(100);
  }



}
  

