#include <ADS1115_WE.h>
#include <Wire.h>
#define I2C_ADDRESS_1 0x48
// #define I2C_ADDRESS_2 0x49
// #define I2C_ADDRESS_3 0x4A

ADS1115_WE adc1 = ADS1115_WE(I2C_ADDRESS_1);
// ADS1115_WE adc2 = ADS1115_WE(I2C_ADDRESS_2);
// ADS1115_WE adc3 = ADS1115_WE(I2C_ADDRESS_3);


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  // ESC.attach(pin);
  Serial.begin(9600);

  if (!adc1.init()) {
    Serial.println("ADS1115-1 not connected!");
  }

  // if(!adc2.init()){
  //   Serial.println("ADS1115-2 not connected!");
  // }

  // if(!adc3.init()){
  //   Serial.println("ADS1115-3 not connected!");
  // }

  Serial.println("Setup Done!");
}

float readChannel(ADS1115_WE adc, ADS1115_MUX channel) {
  float v = 0.0;
  adc.setCompareChannels(channel);
  float voltage = adc.getResult_V();  // alternative: getResult_mV for Millivolt
  return voltage;
}

void loop() {
  float voltage = 0.0;
  voltage = readChannel(adc1, ADS1115_COMP_0_1);
  // Serial.print("ADC1: 1 and GND                [mV]: ");
  Serial.println(voltage);

  voltage = readChannel(adc1, ADS1115_COMP_0_3);
  // Serial.print("ADC1: 3 and GND                [mV]: ");
  Serial.println(voltage);

  delay(1000);
}
