#include <LiquidCrystal.h>
#include <Wire.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <NeoSWSerial.h>

const int rs = 9, en = 8, d4 = 5, d5 = 4, d6 = 3, d7 = 2,   //define pin numbers for LCD
          rows = 20, columns = 4,                           //define LCD dimensions
          solar_in = 0, batt_in = 1, load_in = 2,           //define pin numbers for analog inputs
          ADC_Vref = 5, ADC_bit_res = 10;                   //initialize values for ADC
const byte RF_address[6] = "00001";
//char text[];
float S1_Volts = 0, Batt_Volts = 0, S2_Volts = 0,           //declare vars to store voltage conversion results
      Batt_Percent = 0, P_Charge = 0, P_Load = 0,           //declare vars to store processed data
      local_time_hr = 0;                                    //time in hours
float latitude_deg, longitude_deg, boat_speed_knots;        // S and W is neg
LiquidCrystal screen(rs, en, d4, d5, d6, d7);
RF24 transmitter(7,10);                                      //CE and CSN
int RXPin = 6, TXPin = 7,                                   //RX and TX pins on Arduino with GPS
    GPSBaud = 9600,                                         //GPS baud rate
    date_day = 1, date_month = 1, date_year = 2000;         //date variables
TinyGPSPlus gps;
NeoSWSerial gpsSerial(RXPin, TXPin);

void update_GPS(){
  if(gpsSerial.available() > 0){

      //Serial.println(gps.charsProcessed()); //– the total number of characters received by the object
      //Serial.println(gps.sentencesWithFix()); //– the number of $GPRMC or $GPGGA sentences that had a fix
      //Serial.println(gps.failedChecksum()); //– the number of sentences of all types that failed the checksum test
      //Serial.println(gps.passedChecksum()); //– the number of sentences of all types that passed the checksum test
    if(gps.encode(gpsSerial.read())){
      /*//read latitude and longitude
      if(gps.location.isValid()){    
        latitude_deg = gps.location.lat();
        longitude_deg = gps.location.lng();
        
        Serial.print("Latitude: ");
        Serial.println(latitude_deg);
        Serial.print("Longitude: ");
        Serial.println(longitude_deg);
      }*/

      //read boat speed in knots
      if (gps.speed.isUpdated()) {
        boat_speed_knots = gps.speed.knots();
        /*Serial.print("Boat Speed: ");
        Serial.print(boat_speed_knots);
        Serial.println(" knots");*/
      }
      
      if(gps.date.isUpdated()){ 
        date_month = gps.date.month();
        date_day = gps.date.day();
        date_year = gps.date.year();
        
        /*Serial.print(date_month);
        Serial.print('/');
        Serial.print(date_day);
        Serial.print('/');
        Serial.println(date_year);   */
      }
    
      if(gps.time.isUpdated()){
        local_time_hr = gps.time.hour() + (gps.time.minute()/60);
        
        //Serial.print("Local Time: ");
        //Serial.println(local_time_hr);
      }
    }
    
   /* while (millis() > 5000 && gps.charsProcessed() < 10)
      Serial.println(F("No GPS detected: check wiring."));*/
  }
}

void read_voltage() {
  //read analog inputs through ADC and convert back to voltage
  S1_Volts = ((analogRead(solar_in) * ADC_Vref) / (pow(2,ADC_bit_res) - 1));                                
  Batt_Volts = ((analogRead(batt_in) * ADC_Vref) / (pow(2,ADC_bit_res) - 1));
  S2_Volts = ((analogRead(load_in) * ADC_Vref) / (pow(2,ADC_bit_res) - 1));

  //for testing accuracy of voltage readings
   /*
  Serial.println('A' + String(solar_in) + ": " + String(S1_Volts) + '\n');
  Serial.println('A' + String(batt_in) + ": " + String(Batt_Volts) + '\n');
  Serial.println('A' + String(load_in) + ": " + String(S2_Volts) + '\n');
   */
}

void calculate_voltage_power() {
  //convert analog input back to voltage prior to steppping down
  Batt_Volts *= 38.126;
  Batt_Volts /= 5.086;
  //Serial.println('A' + String(batt_in) + ": " + String(Batt_Volts) + '\n');
  S1_Volts *= 38.604;
  S1_Volts /= 5.093;
  S2_Volts *= 38.814;
  S2_Volts /= 5.094;

  //calculate recorded parameters from raw voltages
  Batt_Percent = ((Batt_Volts - 32.7) / (38.7 - 32.7)) * 100;
  if (Batt_Percent < 0) 
    Batt_Percent = 0;
  P_Charge = ((S1_Volts - Batt_Volts) * Batt_Volts /0.001);
  P_Load = ((Batt_Volts - S2_Volts) * Batt_Volts /0.001);
}

void update_screen() {
  screen.clear();                                                                          //reset cursor to top left and clear screen
  screen.print("Batt: " + String(Batt_Volts, 2) + "V  " + String(Batt_Percent, 0) + "%");  //print Battery charge percent
  screen.setCursor(0,1);                                                                   //move cursor to next line
  screen.print("Load Pow: " + String(P_Load, 2) + "W");                                   //print power supplied by solar panels
  screen.setCursor(0,2);                                                                   //move cursor to next line
  screen.print("Solar Pow: " + String(P_Charge, 2) + "W");                                //print power consumed by load
  //screen.setCursor(0,3);                                                                   //move cursor to next line
  //screen.print("Speed: " + String(boat_speed_knots, 2) + " knots");                        //print boat speed in knots
}

void transmit_data() {
  transmitter.write(&Batt_Volts, sizeof(Batt_Volts));
  //Serial.println("Batt_Volts Transmitted");
  transmitter.write(&P_Load, sizeof(P_Load));
  //Serial.println("P_Load Transmitted");
  transmitter.write(&P_Charge, sizeof(P_Charge));
  //Serial.println("P_Charge Transmitted");
  //transmitter.write(&boat_speed_knots, sizeof(boat_speed_knots));
  //Serial.println("Boat speed Transmitted");
}
  
void setup() {
  //pinMode(7, OUTPUT);
  //pinMode(9, OUTPUT);
  //gpsSerial.begin(GPSBaud);
  screen.begin(rows,columns);                               //set LCD dimensions
  transmitter.begin();
  transmitter.openWritingPipe(RF_address);
  transmitter.stopListening();
  //Serial.begin(9600);
}

void loop() {
  read_voltage();
  calculate_voltage_power();
  //update_GPS();
  update_screen();
  transmit_data();
  delay(5000);                                               //delay for 5 min in ms
}
