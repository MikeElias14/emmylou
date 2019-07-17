#include <MIDI.h>
#include <Wire.h>
#include <Nunchuk.h> // made some modifications to make it easier to use what we need
#include <Adafruit_MPR121.h>

/* MIDI */

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); //TODO: Confirm channel 2 creation / operation
const int channel1 = 1;
const int channel2 = 2;

/* MIDI SETTINGS - CHANGE THESE AS YOU LIKE */
int sustainVal = 0; //determines how long note continues after you lift your finger
int bendThreshold = 0; //determines how easily it will detect a bend
int pent1BottomThresh = 670; //lowest analogRead value of first softpot
int pent1TopThresh = 1024; //highest analogRead value of first softpot
int pent2BottomThresh = 670; //lowest analogRead value of second softpot
int pent2TopThresh = 1024; //highest analogRead value of second softpot

//Vars for first softpot
int posPin1 = A1; //analog
int pos1 = 0; // 0 - 1024
int note1 = 0;
int oldNote1 = 0;
int bend1 = 0;
int oldBend1 = 0;
boolean fingerDown1 = false;
int pos1BaseNote = 65;

//Vars for second softpot
int posPin2 = A2; //analog
int pos2 = 0; // 0 - 1024
int note2 = 0;
int oldNote2 = 0;
int bend2 = 0;
int oldBend2 = 0;
boolean fingerDown2 = false;
int pos2BaseNote = 57;

/* Default ASDR values */
int current_decay = 64 // default is 0x40 (65 decimal) and ranges from 0 to 127
int current_attack = 64


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

/* nunchuk */
int posX = 0;
int posY = 0;

void setup() {
  Serial.begin(9600);
  usbMIDI.begin();
  Wire.begin();

  //MIDI
  pinMode(posPin1, INPUT_PULLDOWN);
  pinMode(posPin2, INPUT_PULLDOWN);
  usbMIDI.sendProgramChange(41, channel1); //bass
  usbMIDI.sendProgramChange(41, channel2); //bass
  usbMIDI.sendProgramChange(124, 0, channel1);
  usbMIDI.sendProgramChange(124, 0, channel2);

  
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

    if(posX < 0  && current_decay > 0) {
      // decrease decay value
      current_decay --;
      usbMIDI.sendControlChange(byte(75), byte(current_decay), channel1);
      usbMIDI.sendControlChange(byte(75), byte(current_decay), channel2);
    }
    else if (posX > 0 && current_decay < 127) {
      current_decay ++;
      usbMIDI.sendControlChange(byte(75), byte(current_decay), channel1);
      usbMIDI.sendControlChange(byte(75), byte(current_decay), channel2);
    }
    if(posY < 0 && current_attack > 0) {
      current_attack --;
      usbMIDI.sendControlChange(byte(73), byte(current_attack), channel1);
      usbMIDI.sendControlChange(byte(73), byte(current_attack), channel2);
    }
    else if (posY > 0 && current_attack < 127) {
      current_attack ++;
      usbMIDI.sendControlChange(byte(73), byte(current_attack), channel1);
      usbMIDI.sendControlChange(byte(73), byte(current_attack), channel2);
    }
    
  }

  /* Cap */
  cap1 = capSense.filteredData(capSensePin1);
  cap2 = capSense.filteredData(capSensePin2);

  vel1 = map(cap1, base1, 0, 127, 0);
  vel2 = map(cap2, base2, 0, 127, 0);

  /* Play Note */
  play(pos1, posPin1, fingerDown1, note1, oldNote1, bend1, oldBend1, pent1BottomThresh, pent1TopThresh, pos1BaseNote, channel1);
  play(pos2, posPin2, fingerDown2, note2, oldNote2, bend2, oldBend2, pent2BottomThresh, pent2TopThresh, pos2BaseNote, channel2);
  /* Print Raw Valuese for Debug */
  printInfo();
  
  delay(10);
}

void play(int pos, int posPin, boolean& fingerDown, int note, int& oldNote, int bend, int& oldBend, int pentBottomThresh, int pentTopThresh, int baseNote, int channel) {
  pos = analogRead(posPin);
  note = map(pos, pentBottomThresh, pentTopThresh, 60, 80);
  bend = map(note, 60, 80, 0, 6383);

  if (note > 60 && note < 80) {
    if (!fingerDown) {
      usbMIDI.sendNoteOn(baseNote, 100, channel);
      usbMIDI.sendPitchBend(bend, channel);
      fingerDown = true;
      delay(10);
      oldNote = note;
      oldBend = bend;
    }
    else if (bend > (oldBend + bendThreshold) || bend < (oldBend - bendThreshold)) {
      usbMIDI.sendPitchBend(bend, channel);
      delay(10);
      oldBend = bend;
     }
   } else if (note < 60 || note > 80) {
     fingerDown = false;
     delay(sustainVal);
     usbMIDI.sendNoteOff(baseNote, 100, channel);
   }
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
