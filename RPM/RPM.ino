#include <arduino-timer.h>

auto timer = timer_create_default();

int pin = 2;
int isObstacle = HIGH; // HIGH = no obstacle 
bool obstacleChange = false;
int RPM = 0;

bool reset() {
  Serial.println(RPM);  // send data
  RPM = 0;
  return true;
}

void setup() {
  Serial.begin(9700);
  pinMode(pin, INPUT);
  timer.every(60000, reset);
}

void loop() {
  timer.tick();
  isObstacle = digitalRead(pin);
  if (obstacleChange && isObstacle == HIGH) { // true and 0 (1 to 0)
    obstacleChange = !obstacleChange; // change obstacleChange back to false  
  } else if (!obstacleChange && isObstacle != HIGH) { // false and 1 (no obstacle to obstacle)
    RPM++;
    obstacleChange = !obstacleChange; // change obstacleChange to true
  }
}
