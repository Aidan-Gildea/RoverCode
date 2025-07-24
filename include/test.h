#include <Arduino.h>

// arbitrary speed values for motors

#define BR_SPEED 253
#define BL_SPEED 255
#define TL_SPEED 122
#define TR_SPEED 120

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
        // Trigger the ultrasonic sensor
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);

        // Set the trigPin HIGH for 10 microseconds
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        // Read the echoPin and calculate the duration
        unsigned long duration = pulseIn(echoPin, HIGH, 25000);

        // Calculate the distance in centimeters
        unsigned long distance = duration * 0.034 / 2;
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


void DriveForward(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight) {
    topLeft.driveForward();
    topRight.driveForward();
    backLeft.driveForward();
    backRight.driveForward();
}

void DriveBackward(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight) {
    topLeft.driveBackward();
    topRight.driveBackward();
    backLeft.driveBackward();
    backRight.driveBackward();
}

void StrafeLeft(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight) {
    topLeft.driveBackward();
    topRight.driveForward();
    backLeft.driveForward();
    backRight.driveBackward();
}

void StrafeRight(HBridgeMotor& topLeft, HBridgeMotor& topRight, HBridgeMotor& backLeft, HBridgeMotor& backRight) {
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
