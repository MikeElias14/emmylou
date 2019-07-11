#include <Adafruit_MPR121.h>



/*********************************************************
This is a library for the MPR121 12-channel Capacitive touch sensor

Designed specifically to work with the MPR121 Breakout in the Adafruit shop 
  ----> https://www.adafruit.com/products/

These sensors use I2C communicate, at least 2 pins are required 
to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.  
BSD license, all text above must be included in any redistribution
**********************************************************/

#include <Wire.h>

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

int sensorPin = 11;

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;
uint16_t base = 0;
double pressure = 0;
uint16_t data = 0;

void setup() {
  Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  Calibrate();
}

void loop() {

  data = cap.filteredData(sensorPin);
  
  /*if (cap.touched() & _BV(sensorPin)){
    pressure = double(double(base)/double(data));
    Serial.print(data);
    Serial.print("\t");
    Serial.println(pressure);
  }*/

  pressure = double(double(base)/double(data));
  Serial.print(data);
  Serial.print("\t");
  Serial.println(pressure);
  
  

  delay(1000);
  return;
}

void Calibrate(){
  // Need a bit of delay so that MPR121 has time to start up
  delay(400);
  base = cap.filteredData(sensorPin);
  Serial.println(base);
}
