#include <MIDI.h>
#include <Wire.h>
#include <Nunchuk.h> // made some modifications to make it easier to use what we need
#include <Adafruit_MPR121.h>

/* MIDI */

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); //TODO: Confirm channel 2 creation / operation
const int channel1 = 1;
const int channel2 = 2;

/* Force */

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

int capSensePin1 = 10;
int capSensePin2 = 11;

// You can have up to 4 on one i2c
Adafruit_MPR121 capSense = Adafruit_MPR121();

uint16_t base1 = 0;
uint16_t base2 = 0;

uint16_t cap1 = 0;
uint16_t cap2 = 0;

int vel1 = 0; // 1-127
int vel2 = 0; // 1-127

/* Position */

int posPin1 = 0; // Analog
int posPin2 = 1; // Analog

int pos1 = 0; // 0 - 1024
int pos2 = 0; // 0 - 1024

int bend1 = 0; // 0 - 16383
int bend2 = 0; // 0 - 16383

/* nunchuk */
int posX = 0;
int posY = 0;

void setup() {
  Serial.begin(9600);
  usbMIDI.begin();
  Wire.begin();

  // Change TWI speed for nuchuk, which uses Fast-TWI (400kHz)
  Wire.setClock(400000);
  
  nunchuk_init();

  if (!capSense.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  
  Serial.println("MPR121 found!");
  Calibrate();

  /* Set Up Midi */

  /* MIDI Bass sound */
  usbMIDI.sendProgramChange(36, channel1);
  usbMIDI.sendProgramChange(36, channel2);
  
  /* Pitchbend one octave */
  usbMIDI.sendControlChange(6, 6, channel1);
  usbMIDI.sendControlChange(101, 0, channel1);
  usbMIDI.sendControlChange(100, 0, channel1);

  usbMIDI.sendControlChange(6, 6, channel2);
  usbMIDI.sendControlChange(101, 0, channel2);
  usbMIDI.sendControlChange(100, 0, channel2);

  // TODO - Allyson: configure the ADSR for both channels.
    
}

void loop() {
  
  /* Position */  
  pos1 = analogRead(posPin1);
  pos2 = analogRead(posPin2);

  bend1 = map(pos1, 0, 1024, 0, 16383);
  bend2 = map(pos2, 0, 1024, 0, 16383);
  
  /* Nunchuk */
  if (nunchuk_read()) {
    // Work with nunchuk_data 
    posX = nunchuk_joystickX(); // +/- 100
    posY = nunchuk_joystickY(); // +/- 100

    // TODO: Map the nunchuk x / y to a modulation or distortion and send as part of the midi packet 
  }

  /* Cap */
  cap1 = capSense.filteredData(capSensePin1);
  cap2 = capSense.filteredData(capSensePin2);

  vel1 = map(cap1, base1, 0, 127, 0);
  vel2 = map(cap2, base2, 0, 127, 0);

  /* Play Note */
  // TODO - Allyson: Create the play note loops based on the sensed vel / bend
    // Req: - Send on
        //  - update pitchbend and modulation / distortion
        //  - Once note is released send off

  /* Print Raw Valuese for Debug */
  printInfo();
  
  delay(10);
}

void printInfo() {
  Serial.print("Pos1: ");
  Serial.print(pos1);
  Serial.print(" \t Pos2: ");
  Serial.print(pos2);
  Serial.print(" \t Cap1: ");
  Serial.print(cap1);
  Serial.print(" \t Cap2: ");
  Serial.print(cap2);
  Serial.print(" \t Nunchuk: ");
  nunchuk_print();
  
  Serial.println("");
}

void Calibrate(){
  // Need a bit of delay so that MPR121 has time to start up
  delay(400);
  
  base1 = capSense.filteredData(capSensePin1);
  base2 = capSense.filteredData(capSensePin2);
  
  Serial.print("Base1: " );
  Serial.println(base1);
  Serial.print("Base2: " );
  Serial.println(base2);
}
