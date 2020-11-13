const int mvPerAmp = 100;
const int offsetVoltage = 2500;

const int currentSensorAnalogPin = A0;
const int voltageSensorAnalogPin = A1;
const int sensorIRDigitalPin = 2;

// HIGH is no obstacle
int currentIRSensorState = HIGH; 

unsigned long prevTime; //In Micro Seconds
unsigned long timeBetweenRev; // In Micro Seconds

float revPerMicroSec;
float rpm;
long current;
float voltage;

void setup() {
  pinMode(sensorIRDigitalPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  currentIRSensorState = digitalRead(sensorIRDigitalPin);
  
  if(currentIRSensorState == LOW) {
    timeBetweenRev = micros() - prevTime;
    prevTime = micros();
    revPerMicroSec = (2 * PI) / timeBetweenRev;
    rpm = (revPerMicroSec / pow(10, -6)) * 60;
  }

  current = analogRead(currentSensorAnalogPin);
  current = (current / 1024.0) * 5000;
  current = (current - offsetVoltage) / mvPerAmp;
  //The sensor can apparently be offset by some  varying value, so this requires testing

  voltage = analogRead(voltageSensorAnalogPin);
  voltage = (voltage / 1023.0) * 5;

  Serial.print(String(rpm) + "," + String(current) + "," + String(voltage));
}
