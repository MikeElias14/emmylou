#include <CapacitiveSensor.h>
#include <MIDI.h>

/* Force */
CapacitiveSensor string = CapacitiveSensor(4, 2); // pin 2 is sensor pin
int capMax = 0;
int capMin = 50;
int cap = 0;
int vel = 0; // 1-127


/* MIDI SETTINGS - CHANGE THESE AS YOU LIKE */
int sustainVal = 0; //determines how long note continues after you lift your finger
int bendThreshold = 0; //determines how easily it will detect a bend
int pent1BottomThresh = 670; //lowest analogRead value of first softpot
int pent1TopThresh = 1024; //highest analogRead value of first softpot
int pent2BottomThresh = 670; //lowest analogRead value of second softpot
int pent2TopThresh = 1024; //highest analogRead value of second softpot

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
const int channel = 1;

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


void setup() {
  Serial.begin(9600);
  usbMIDI.begin();
  pinMode(posPin1, INPUT_PULLDOWN);
  pinMode(posPin2, INPUT_PULLDOWN);
  usbMIDI.sendProgramChange(41, channel); //bass
  usbMIDI.sendControlChange(6, 6, channel);
  usbMIDI.sendControlChange(101, 0, channel);
  usbMIDI.sendControlChange(100, 0, channel);
}

void loop() {
  play(pos1, posPin1, fingerDown1, note1, oldNote1, bend1, oldBend1, pent1BottomThresh, pent1TopThresh, pos1BaseNote);
  play(pos2, posPin2, fingerDown2, note2, oldNote2, bend2, oldBend2, pent2BottomThresh, pent2TopThresh, pos2BaseNote);

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

}

void play(int pos, int posPin, boolean& fingerDown, int note, int& oldNote, int bend, int& oldBend, int pentBottomThresh, int pentTopThresh, int baseNote) {
  pos = analogRead(posPin);
//  Serial.print("pos: ");
//  Serial.print(pos);
//  Serial.print("\n");

  note = map(pos, pentBottomThresh, pentTopThresh, 60, 80);
  Serial.print("note: ");
  Serial.print(note);
  Serial.print("\n");

  bend = map(note, 60, 80, 0, 6383);
//  Serial.print("map: ");
//  Serial.print(bend);
//  Serial.print("\n");
//  
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
