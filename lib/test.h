#include <Arduino.h>

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
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        // Measure the duration of the echo
        unsigned long duration = pulseIn(echoPin, HIGH);

        unsigned long distance = duration * 0.034 / 2;

        return distance;
    }
};



