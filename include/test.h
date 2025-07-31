#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_light.h>

// speed values for going forward -- done 

#define BR_SPEED 250
#define BL_SPEED 255
#define TL_SPEED 126
#define TR_SPEED 132

//regular motor speeds for strafing right-- done
#define BR_R_SPEED 255
#define BL_R_SPEED 255
#define TL_R_SPEED 145
#define TR_R_SPEED 105

//regular motor speeds for strafing left--done
#define BR_L_SPEED 240
#define BL_L_SPEED 238
#define TL_L_SPEED 105
#define TR_L_SPEED 120


// slower motor speeds for object driving forward / backward -- done

#define SLOW_BR_FB 165
#define SLOW_BL_FB 148
#define SLOW_TL_FB 65
#define SLOW_TR_FB 80


// ---------- currently not using ------------

// now for strafing left -- done
#define SLOW_BR_SL 180
#define SLOW_BL_SL 170
#define SLOW_TL_SL 85
#define SLOW_TR_SL 100

//now for strafing right -- done
#define SLOW_BR_SR 135
#define SLOW_BL_SR 105
#define SLOW_TL_SR 135
#define SLOW_TR_SR 160

// ------------------------------------------
//now test for strafing right without wheel
// #define SLOW_BR_FB 200
// #define SLOW_BL_FB 190
// #define SLOW_TL_FB 105
// #define SLOW_TR_FB 107

// more changes !

struct LED
{
    int pin;
    bool state;

    LED(int p) : pin(p), state(false) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    void on() {
        state = true;
        digitalWrite(pin, HIGH);
    }

    void off() {
        state = false;
        digitalWrite(pin, LOW);
    }

    void toggle() {
        state = !state;
        digitalWrite(pin, state ? HIGH : LOW);
    }
};

struct Timer {
private:
  unsigned long previousTime;
  unsigned long interval;

public:
  Timer(unsigned long i = 1000) : previousTime(0), interval(i) {}

  void setInterval(unsigned long i) {
      interval = i;
    }

  bool isReady() {
    unsigned long currentTime = millis();
    if (currentTime - previousTime >= interval) {
      return true;
    }
    return false;
  }

  void reset() {
    previousTime = millis();
  }
};


struct Ultrasonic
{
    int trigPin;
    int echoPin;

    Ultrasonic(int trig, int echo) : trigPin(trig), echoPin(echo) {
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
        digitalWrite(trigPin, LOW); // this is for resetting the trig pin .
    }

    long readDistance() {
        unsigned long distance = 0;
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);

        // Set the trigPin HIGH for 10 microseconds
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        // Read the echoPin and calculate the duration
        unsigned long duration = pulseIn(echoPin, HIGH);

        // Calculate the distance in centimeters
        distance += duration * 0.034 / 2;
            
        delay(50);
        return distance; // return the average distance in cm
    }
};


struct HBridgeMotor
{
    int enPin;
    int in1Pin;
    int in2Pin;

    HBridgeMotor(int en, int in1, int in2) : enPin(en), in1Pin(in1), in2Pin(in2) {
        pinMode(enPin, OUTPUT);
        pinMode(in1Pin, OUTPUT);
        pinMode(in2Pin, OUTPUT);
        digitalWrite(enPin, LOW);
        digitalWrite(in1Pin, LOW);
        digitalWrite(in2Pin, LOW);
    }

    void setMotorSpeed(int speed) {
        analogWrite(enPin, speed);
    }

    void driveForward() {
        digitalWrite(in1Pin, HIGH);
        digitalWrite(in2Pin, LOW);
    }

    void driveBackward() {
        digitalWrite(in1Pin, LOW);
        digitalWrite(in2Pin, HIGH);
    }

    void stop() {
        digitalWrite(in1Pin, LOW);
        digitalWrite(in2Pin, LOW);
    }
};

void SetSpeeds(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight, byte tl, byte tr, byte bl, byte br)
{
  topLeft.setMotorSpeed(tl);
  topRight.setMotorSpeed(tr);
  backLeft.setMotorSpeed(bl);
  backRight.setMotorSpeed(br);
}


// unfinished :(
struct Encoder
{
    int sensorPin;
    int currentState;
    int previousState;
    volatile int tickCount;

    Encoder(int a, int b) : sensorPin(a), currentState(0), previousState(0), tickCount(0) {
        pinMode(sensorPin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(sensorPin), &Encoder::updatePosition, CHANGE);
    }

    static void updatePosition() {
        // This function will be called on interrupt
        // Logic to update position based on pin states
        // Needs to be implemented based on specific encoder type
    }
};


void DriveForward(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight, bool mazenav) {
    if(mazenav == true)
    {
        // maze navigation speeds
        SetSpeeds(topLeft, topRight, backLeft, backRight, TL_SPEED, TR_SPEED, BL_SPEED, BR_SPEED);   
    }
    else
    {
        SetSpeeds(topLeft, topRight, backLeft, backRight, SLOW_TL_FB, SLOW_TR_FB, SLOW_BL_FB, SLOW_BR_FB);  
    }
        
    topLeft.driveForward();
    topRight.driveForward();
    backLeft.driveForward();
    backRight.driveForward();
}

void DriveBackward(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight, bool mazenav) {
    if(mazenav == true)
    {
        SetSpeeds(topLeft, topRight, backLeft, backRight, TL_SPEED, TR_SPEED, BL_SPEED, BR_SPEED);   
    }
    else
    {
        SetSpeeds(topLeft, topRight, backLeft, backRight, SLOW_TL_FB, SLOW_TR_FB, SLOW_BL_FB, SLOW_BR_FB);  
    }
    
    topLeft.driveBackward();
    topRight.driveBackward();
    backLeft.driveBackward();
    backRight.driveBackward();
}


void StrafeLeft(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight, bool mazenav) {
    
    if(mazenav == true)
    {
        SetSpeeds(topLeft, topRight, backLeft, backRight, TL_L_SPEED, TR_L_SPEED, BL_L_SPEED, BR_L_SPEED);   
    }
    else
        {
            SetSpeeds(topLeft, topRight, backLeft, backRight, TL_L_SPEED, TR_L_SPEED, BL_L_SPEED, BR_L_SPEED);  
    }
    
    topLeft.driveBackward();
    topRight.driveForward();
    backLeft.driveForward();
    backRight.driveBackward();
}

void StrafeRight(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight, bool mazenav) {
    if(mazenav == true)
    {
        SetSpeeds(topLeft, topRight, backLeft, backRight, TL_R_SPEED, TR_R_SPEED, BL_R_SPEED, BR_R_SPEED);   
    }
    else
    {
        SetSpeeds(topLeft, topRight, backLeft, backRight, SLOW_TL_SR, TR_R_SPEED, BL_R_SPEED, BR_R_SPEED);  
    }
    
    topLeft.driveForward();
    topRight.driveBackward();
    backLeft.driveBackward();
    backRight.driveForward();
}

void StopMotors(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight) {
    topLeft.stop();
    topRight.stop();
    backLeft.stop();
    backRight.stop();
}

// void SpinRight(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight) {
//     topLeft.driveForward();
//     topRight.driveBackward();
//     backLeft.driveForward();
//     backRight.driveBackward();
// }

// void SpinLeft(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight) {
//     topLeft.driveBackward();
//     topRight.driveForward();
//     backLeft.driveBackward();
//     backRight.driveForward();
// }


bool TestUltrasonic(Ultrasonic& sensor, int flagDistance) {


 long distance = sensor.readDistance();
 if(distance < flagDistance) {
   return true;
 }
 return false;


}

