#include <arduino-timer.h>

auto timer = timer_create_default();

int pin = 2;
int isObstacle = HIGH; // HIGH = no obstacle 
bool obstacleChange = false;
int RPM = 0;

bool reset() {
  Serial.println(RPM * 20);
  RPM = 0;
  return true;
}

void setup() {
  Serial.begin(9700);
  timer.every(3000, reset);
}

void loop() {
  timer.tick();
  isObstacle = digitalRead(pin);
  if (obstacleChange && isObstacle == HIGH) {
    obstacleChange = !obstacleChange;   
  } else if (!obstacleChange && isObstacle != HIGH) {
    RPM++;
    obstacleChange = !obstacleChange;
  }
}
