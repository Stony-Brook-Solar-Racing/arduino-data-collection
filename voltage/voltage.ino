float vOut = 0.0;
float vIn = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int value = 0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  value = analogRead(A0);
  vOut = (value * 5.0) / 1024.0;
  vIn = vOut / (R2/(R1=R2));
  Serial.println(vIn);
  delay(100);
}
