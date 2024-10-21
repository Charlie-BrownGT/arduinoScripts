#include <Stepper.h>

double stepsPerRevolution = 2048; 
double deg = stepsPerRevolution / 360;
double angStep = 45; 
double eachStep = angStep * deg;

int rpm = 10;

//initialize stepper library on the right pins, pin order IN1, IN3, IN2, IN4
Stepper myStepper (stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  // put your setup code here, to run once:
  myStepper.setSpeed(rpm);

}

void loop() {
  // put your main code here, to run repeatedly:
  myStepper.step(eachStep);
  delay(500);
}
