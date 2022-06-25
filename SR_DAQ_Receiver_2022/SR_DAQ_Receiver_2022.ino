#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>

RF24 receiver(9, 10);
const byte address[6] = "00001";
float data;

void setup() {
  Serial.begin(9600);
  receiver.begin();
  //receiver.printDetails();
  receiver.openReadingPipe(0, address);
  receiver.startListening();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (receiver.available()) {
    receiver.read(&data, sizeof(data));
    Serial.println(data);
    Serial.print('\n');
  }
}
