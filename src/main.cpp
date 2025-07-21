#include <Arduino.h>
#include <test.h>

#define sensorPin 12

#define enA_1 7
#define in1_1 6
#define in2_1 5

HBridge hBridge(enA_1, in1_1, in2_1);


int currentState;
int previousState;
volatile int tickCount;

unsigned long lastTickTime;
unsigned long currentTickTime;
unsigned long deltaTime;
float rpm;


void setup() {
  Serial.begin(9600);

  pinMode(sensorPin, INPUT);

  hBridge.setMotorSpeed(255);
  hBridge.driveForward();

}

void loop()
{
  currentState = digitalRead(sensorPin);

  if (currentState != previousState) {
    if (currentState == HIGH) {
      tickCount++;
      currentTickTime = millis();
      deltaTime = currentTickTime - lastTickTime;
      lastTickTime = currentTickTime;

      rpm = (tickCount / (deltaTime / 60000.0)); // Convert to RPM
      Serial.print("RPM: ");
      Serial.println(rpm);
    }
  }

  previousState = currentState;

  delay(100); // Adjust delay as needed
  

}
