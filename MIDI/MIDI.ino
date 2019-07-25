#include <MIDI.h>
#include <Wire.h>
#include <Nunchuk.h> // Modified Arduino Nunchuk header that can be useed with Teensy 3.5

/* MIDI */

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
const int channel1 = 1;
const int channel2 = 2;

/* init vars for SoftPot / Position */

int posPin1 = A1;
int posPin2 = A0;

int oldBend1 = 0;
int oldBend2 = 0;

boolean fingerDown1 = false;
boolean fingerDown2 = false;

// The note to send via MIDI, change this to change the tuning 
int pos1BaseNote = 65;
int pos2BaseNote = 57;

/* Default MIDI-control values */
int current_control1 = 64; // default is 0x40 (65 decimal) and ranges from 0 to 127
int current_control2 = 64;

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
  
  // Change clock speed for Nunchuk 400kHz)
  Wire.setClock(400000);
  nunchuk_init();

  /* Set Up Midi */
  usbMIDI.sendProgramChange(36, channel1); //bass
  usbMIDI.sendProgramChange(36, channel2); //bass
  
  /* Pitchbend one octave */
  usbMIDI.sendControlChange(6, 6, channel1);
  usbMIDI.sendControlChange(101, 0, channel1);
  usbMIDI.sendControlChange(100, 0, channel1);

  usbMIDI.sendControlChange(6, 6, channel2);
  usbMIDI.sendControlChange(101, 0, channel2);
  usbMIDI.sendControlChange(100, 0, channel2);
    
}

void loop() {
  
  /* Nunchuk */
  if (nunchuk_read()) {
    posX = nunchuk_joystickX(); // +/- 100
    posY = nunchuk_joystickY(); // +/- 100

    if(posX < -10  && current_control1 > 0) {
      // If joystick is on the negative X position, decrement the control1 value (if it is not at the minimum already)
      current_control1 --;
      usbMIDI.sendControlChange(byte(75), byte(current_control1), channel1);
      usbMIDI.sendControlChange(byte(75), byte(current_control1), channel2);
    }
    else if (posX > 10 && current_control1 < 127) {
      // If joystick is on the positive X position, increment the control1 value (if it is not at the maximum already)
      current_control1 ++;
      usbMIDI.sendControlChange(byte(75), byte(current_control1), channel1);
      usbMIDI.sendControlChange(byte(75), byte(current_control1), channel2);
    }
    if(posY < -10 && current_control2 > 0) {
      // If joystick is on the negative Y position, decrement the control2 value (if it is not at the minimum already)
      current_control2 --;
      usbMIDI.sendControlChange(byte(73), byte(current_control2), channel1);
      usbMIDI.sendControlChange(byte(73), byte(current_control2), channel2);
    }
    else if (posY > 10 && current_control2 < 127) {
      // If joystick is on the positive Y position, increment the control2 value (if it is not at the maximum already)
      current_control2 ++;
      usbMIDI.sendControlChange(byte(73), byte(current_control2), channel1);
      usbMIDI.sendControlChange(byte(73), byte(current_control2), channel2);
    }
  }
  else{
    // If Nunchuck is not being operated reset the mapped values to default values
    current_control1 = 64;
    current_control2 = 64;
  }

  /* Play Notes */
  play(posPin1, fingerDown1, oldBend1, pos1BaseNote, channel1);
  play(posPin2, fingerDown2, oldBend2, pos2BaseNote, channel2);

  
  /* Print Raw Values for Debug */
  printInfo();
  
  delay(10);
}

void play(int posPin, boolean &fingerDown, int &oldBend, int baseNote, int channel) {

  // Read the lin pots and determine the bend
  int pos = analogRead(posPin);
  int bend = map(pos, 100, 1024, 0, 6383);

  if (!fingerDown && pos > 100) { // If the user is playing then play a note
    usbMIDI.sendNoteOn(baseNote, 100, channel);
    usbMIDI.sendPitchBend(bend, channel);
    
    fingerDown = true;
    delay(10);
    oldBend = bend;
    delay(10);
  }
  else if (bend > (oldBend + 20) || bend < (oldBend - 20)) { // Detect change in bend (position) and send new pitchbend
    usbMIDI.sendPitchBend(bend, channel);
    delay(10);
    oldBend = bend;
   }
 } else if ((abs(nunchuk_joystickX()) <= 10 && abs(nunchuk_joystickY()) <= 10) && fingerDown && pos < 100) { //When the user stops playing
   fingerDown = false;
   usbMIDI.sendNoteOff(baseNote, 100, channel);
}

void printInfo() {
  Serial.print("Pos1: ");
  Serial.print(pos1));
  Serial.print(" \t Pos2: ");
  Serial.print(pos2);
  Serial.print(" \t Cap1: ");
  Serial.print(cap1);
  Serial.print(" \t Cap2: ");
  Serial.print(cap2);
  Serial.print(" \t Vel1: ");
  Serial.print(vel1);
  Serial.print(" \t Vel2: ");
  Serial.print(vel2);  
  Serial.println("");
}
