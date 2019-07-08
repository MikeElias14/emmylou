#include <CapacitiveSensor.h>
#include <MIDI.h>

/* MIDI */
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
const int channel = 1;

/* Force */

CapacitiveSensor string = CapacitiveSensor(4, 2); // pin 2 is sensor pin

int capMax = 0;
int capMin = 50;
int cap = 0;
int vel = 0; // 1-127

int note1 = 0;
int note2 = 0;

/* Position */

int posPin1 = 0; // Analog
int pos1 = 0; // 0 - 1024
int bend1 = 0; // 0 - 16383

int posPin2 = 1; // Analog
int pos2 = 0; // 0 - 1024
int bend2 = 0; // 0 - 16383

int prev1 = 0;
int prev2 = 0;

void setup() {
  Serial.begin(9600);
  usbMIDI.begin();
  delay(100);
  usbMIDI.sendProgramChange(41, channel); //violin
}

void loop() {
  /* Position - Pitchbend*/

//  usbMIDI.sendProgramChange(41, channel); //violin - NOTE jst for my midi thing

  /* for changing pitchbend range. Ideally just put this in the setup, but my midi thing doesnt boot fast enough */
  usbMIDI.sendControlChange(6, 6, channel);
  usbMIDI.sendControlChange(101, 0, channel);
  usbMIDI.sendControlChange(100, 0, channel);
  
  pos1 = analogRead(posPin1);
  pos2 = analogRead(posPin2);

  Serial.print("pos1: ");
  Serial.print(pos1);
  Serial.print("\t");

  Serial.print("pos2: ");
  Serial.print(pos2);
  Serial.print("\t");

  bend1 = map(pos1, 0, 1024, 0, 16383);
  bend2 = map(pos2, 0, 1024, 0, 16383);

  Serial.print("Bend1: ");
  Serial.print(bend1);
  Serial.print("\t");

  Serial.print("Bend2: ");
  Serial.print(bend2);
  Serial.print("\t");

//  /* Cap - Velocity */
//
//  long start = millis();
//  long cap =  string.capacitiveSensor(30);
//
//  Serial.print("Cap: ");
//  Serial.print(cap);
//  Serial.print("\t");
//
//  if (cap > capMax){
//    capMax = cap;
//  }
//
//  if (cap < capMin) {
//    cap = 0;
//  }
//
//  vel = map(cap, 0, capMax, 0, 127);
//
//  Serial.print("Vel: ");
//  Serial.print(vel);
//  Serial.print("\t");


  /* Play Note */
//  if (bend1 >= 1000) {
//    usbMIDI.sendNoteOn(65, 100, channel);
//    usbMIDI.sendPitchBend(bend1, channel);
//    delay(10);
//    usbMIDI.sendNoteOff(65, 100, channel);
//  }

  note2 = map(pos2, 0, 1024, 60, 80);

  if (bend2 > 2000 && ((bend2 + 100) < prev2 || (bend2 - 100) > prev2 )) {
//    usbMIDI.sendNoteOn(57, 100, channel);
//    usbMIDI.sendPitchBend(bend2, channel);
    usbMIDI.sendNoteOn(note2, 100, channel);
    delay(50);
    prev2 = bend2;
//    usbMIDI.sendNoteOff(57, 100, channel); //just constantly send note with velocity. no need to turn off
  }
  

  Serial.println("");
//  delay(100);
}
