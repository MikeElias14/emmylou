// Sample code to create a monophonic synthesizer to run on the SYDE mini synth
// The code only receives midinotes (on any channel) - use the pots to affect the sound
// see https://www.pjrc.com/teensy/gui/index.html for more info on what is possible with the audio library!
// see https://www.pjrc.com/teensy/td_midi.html for more info on using usb-midi with the teensy
// Make sure to set the board type: Tools/Board -> Teensy 3.2
// Make sure to set the USB type: Tools/USB type -> MIDI

// There is some noise present when using headphones and powering from your usb...this due to a ground loop
// which makes your synth and usb-cable act as an antennae. 

// Load some libraries needed to get things running
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
//#include <MIDI.h>

// Define this for the mini-synth LED pin
#define LEDpin 6

// Define pins for the mini-synth multiplexer
#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define zMux A0

const int channel = 1;
int pos1BaseNote = 65;
float pos1BaseFreq = float((440/32.0)*pow(2,float((pos1BaseNote-9))/12)); 
float pitchBend = 0.0;


// GUItool: begin automatically generated code
AudioSynthWaveform       squareWave;      //xy=128.0000114440918,578.80006980896
AudioSynthWaveform       sawtoothWave; //xy=131.60000610351562,628.2000217437744
AudioSynthWaveformSine   lfo;          //xy=173.6000099182129,700.2000217437744
AudioAmplifier           sawtoothWaveAmp; //xy=269.2000198364258,628.600118637085
AudioAmplifier           squareWaveAmp;           //xy=283.6000061035156,579.2000198364258
AudioAmplifier           lfoAmp; //xy=305.6000099182129,699.2000217437744
AudioMixer4              mixer;         //xy=462.60001373291016,616.2000198364258
AudioFilterStateVariable lowPassFilter;        //xy=614.9999351501465,645.80006980896
AudioEffectEnvelope      ADSRenvelope;      //xy=807.0000381469727,672.8001079559326
AudioAmplifier           amp;           //xy=894.4000358581543,734.0000133514404
AudioOutputAnalog        dac;           //xy=895.0000381469727,792.0000600814819
AudioConnection          patchCord1(squareWave, squareWaveAmp);
AudioConnection          patchCord2(sawtoothWave, sawtoothWaveAmp);
AudioConnection          patchCord3(lfo, lfoAmp);
AudioConnection          patchCord4(sawtoothWaveAmp, 0, mixer, 1);
AudioConnection          patchCord5(squareWaveAmp, 0, mixer, 0);
AudioConnection          patchCord6(lfoAmp, 0, lowPassFilter, 1);
AudioConnection          patchCord7(mixer, 0, lowPassFilter, 0);
AudioConnection          patchCord8(lowPassFilter, 0, ADSRenvelope, 0);
AudioConnection          patchCord9(ADSRenvelope, amp);
AudioConnection          patchCord10(amp, dac);
// GUItool: end automatically generated code

elapsedMillis framePeriod = 0; // a timer used to only update things after a certain amount of time
// elapsedMillis is automatically included for Teensy's, but the library needs to be imported for other boards

float midiNotes[128]; // an array to convert midi note numbers to frequency
float dBvol[128];     // an array to set the volume using a dB scale, not a linear scale

int currNote = 0; // a variable to keep track of which MIDI note is on

void setup() {
  Serial.begin(9600);
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  
  AudioMemory(15);                 // set the audio memory
  dac.analogReference(INTERNAL);   // normal volume - "EXTERNAL" is louder

  float a = 440; // a is 440 hz...

  // calculates frequencies associated with MIDI note numbers
  for (int x = 0; x <= 127; x++)
  {
    midiNotes[x] = float((a/32.0)*pow(2,float((x-9))/12)); 
  }

  // calculate a dB volume scale - this isn't technically correct, but a good enough approximation
  for (int x = 0; x <= 127; x++)
  {
    dBvol[x] = pow(float(x)/127,4);
  }
  
  ADSRenvelope.releaseNoteOn(10);
  squareWave.begin(1,10,WAVEFORM_SQUARE);
  sawtoothWave.begin(1,10,WAVEFORM_SAWTOOTH);
  amp.gain(0);
  mixer.gain(0,0.5);
  mixer.gain(1,0.5);   
  pinMode(S0,OUTPUT);
  pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  digitalWrite(S0,0);
  digitalWrite(S1,1);
  digitalWrite(S2,1);
  digitalWrite(S3,0);
  pinMode(LEDpin, OUTPUT);
  analogWrite(LEDpin, 255);
  delay(500);
  analogWrite(LEDpin, 0);
  delay(500);
  analogWrite(LEDpin, 127);
  delay(500);
  analogWrite(LEDpin, 0);
  delay(500);
  analogWrite(LEDpin, 63);
  delay(500);

}

void loop() {
  // Read the MIDI bus for any incoming messages and respond accordingly
  //usbMIDI.read();
  int pos = 0;
  int bend = 0;
  myNoteOn(1, 65, 100);
  delay(1000);

  myPitchBend(1, -8000);
  delay(50);
  myPitchBend(1, -6000);
  delay(50);
  myPitchBend(1, -5000);
  delay(50);
  myPitchBend(1, -4000);
  delay(50);
  myPitchBend(1, -2000);
  delay(50);
  myPitchBend(1, -1000);
  delay(50);
  myPitchBend(1, 0);
  delay(50);
  myPitchBend(1, 1000);
  delay(50);
  myPitchBend(1, 2000);
  delay(50);
  myPitchBend(1, 4000);
  delay(50);
  myPitchBend(1, 5000);
  delay(50);
  myPitchBend(1, 6000);
  delay(50);
  myPitchBend(1, 8000);
  delay(50);
  myPitchBend(1, -8000);
  delay(50);
  myPitchBend(1, -6000);
  delay(50);
  myPitchBend(1, -4000);
  delay(50);
  myPitchBend(1, -2000);
  delay(50);
  myPitchBend(1, 0);
  delay(50);
  myPitchBend(1, 2000);
  delay(50);
  myPitchBend(1, 4000);
  delay(50);
  myPitchBend(1, 6000);
  delay(50);
  myPitchBend(1, 8000);
  delay(50);
  
  
  delay(1000);
  myNoteOff(1,65,100);
  
  // Update the synthesizer with parameters from the potentiometers
  // Only do this after a certain amoutn of time (so we're not always updating it on every loop)
  if(framePeriod>100){
    updatePots();
    framePeriod = 0;   
  }
  
  delay(1000);
}

void myPitchBend(byte channel, int bend) {
  // bend is apparently -8192 to 8192
  // calculate bend ratio
  pitchBend = float(bend / 8192.0);
  float newFreq = pos1BaseFreq + ((pos1BaseFreq /2 ) * pitchBend);

  // Assuming myPitchBend is only ever called after a myNoteOn
  squareWave.frequency(newFreq);
  sawtoothWave.frequency(newFreq);
}


void myNoteOn(byte channel, byte note, byte velocity) {
  // When using MIDIx4 or MIDIx16, usbMIDI.getCable() can be used
  // to read which of the virtual MIDI cables received this message.
  currNote = note;        // update the state of the currNote
  ADSRenvelope.noteOff(); // just to be safe turn off the note whenever a new note received
  analogWrite(LEDpin, 0);
  ADSRenvelope.noteOn();  // turn the note on
  analogWrite(LEDpin, 255);
  // update the oscillators
  squareWave.frequency(midiNotes[note]);
  squareWave.amplitude(0.5*float(velocity)/127);
  sawtoothWave.frequency(midiNotes[note]);
  sawtoothWave.amplitude(0.5*float(velocity)/127);
}

void myNoteOff(byte channel, byte note, byte velocity) {
  if(note == currNote){
    ADSRenvelope.noteOff();
    analogWrite(LEDpin, 0);    
  }
}



 void updatePots(){
  //Read multiplexer pin 0 to attackPot
  digitalWrite(S0,0);
  digitalWrite(S1,0);
  digitalWrite(S2,0);
  digitalWrite(S3,0);
  int attackPot = analogRead(zMux);

  // map attackPot to attack time (min, max)
  ADSRenvelope.attack(map(attackPot,0,1023,10,1000));

  //Read multiplexer pin 1 to decayPot
  digitalWrite(S0,1);
  digitalWrite(S1,0);
  digitalWrite(S2,0);
  digitalWrite(S3,0);
  int decayPot = analogRead(zMux);

  // map decayPot to attack time (min, max)
  ADSRenvelope.decay(map(decayPot,0,1023,5,1000));
  
  //Read multiplexer pin 2 to sustainPot
  digitalWrite(S0,0);
  digitalWrite(S1,1);
  digitalWrite(S2,0);
  digitalWrite(S3,0);
  int sustainPot = analogRead(zMux)/8;

  // map sustainPot to sustain level using dB scale  
  ADSRenvelope.sustain(dBvol[sustainPot]);

  //Read multiplexer pin 3 to releasePot
  digitalWrite(S0,1);
  digitalWrite(S1,1);
  digitalWrite(S2,0);
  digitalWrite(S3,0);
  int releasePot = analogRead(zMux);
 
  // map releasePot to attack time (min, max)  
  ADSRenvelope.release(map(releasePot,0,1023,10,2000));
  
  //Read multiplexer pin 4 to cutoffPot
  digitalWrite(S0,0);
  digitalWrite(S1,0);
  digitalWrite(S2,1);
  digitalWrite(S3,0);
  int cutoffPot = analogRead(zMux);

  // map  cutoffPot to LPF cutoff frequency    
  lowPassFilter.frequency(constrain(midiNotes[currNote]*(float(map(cutoffPot,0,1023,0,2000))/100),0,15000));

  //Read multiplexer pin 5 to resonancePot
  digitalWrite(S0,1);
  digitalWrite(S1,0);
  digitalWrite(S2,1);
  digitalWrite(S3,0);
  int resonancePot = analogRead(zMux);
  
  // map  cutoffPot to LPF resonance - ranges  from 0.7 to 5.0   
  lowPassFilter.resonance(float(map(resonancePot,0,1023,700,5000))/1000);

  //Read multiplexer pin 6 to amplitudePot
  digitalWrite(S0,0);
  digitalWrite(S1,1);
  digitalWrite(S2,1);
  digitalWrite(S3,0);
  int amplitudePot = analogRead(zMux)/8;

  // map amplitudePot to amp level using dB scale    
  amp.gain(dBvol[amplitudePot]);

  //Read multiplexer pin 7 to p1Pot LFO Rate
  digitalWrite(S0,1);
  digitalWrite(S1,1);
  digitalWrite(S2,1);
  digitalWrite(S3,0);
  int p1Pot = analogRead(zMux);
  lfo.frequency(25*float(p1Pot)/1023);

  //Read multiplexer pin 8 to p2Pot LFO Amplitude
  digitalWrite(S0,0);
  digitalWrite(S1,0);
  digitalWrite(S2,0);
  digitalWrite(S3,1);
  int p2Pot = analogRead(zMux)/8;
  lfo.amplitude(dBvol[p2Pot]);

  //Read multiplexer pin 9 to p3Pot Square Wave Amplitude
  digitalWrite(S0,1);
  digitalWrite(S1,0);
  digitalWrite(S2,0);
  digitalWrite(S3,1);
  int p3Pot = analogRead(zMux)/8;

  // map p3Pot to squareWaveAmp level using dB scale
  squareWaveAmp.gain(dBvol[p3Pot]);
  
  //Read multiplexer pin 10 to p4Pot sawtooth Wave Amplitude
  digitalWrite(S0,0);
  digitalWrite(S1,1);
  digitalWrite(S2,0);
  digitalWrite(S3,1);
  int p4Pot = analogRead(zMux)/8;

  // map p3Pot to sawtoothWaveAmp level using dB scale
  sawtoothWaveAmp.gain(dBvol[p4Pot]);
}




// https://forum.pjrc.com/threads/35596-MIDI-controlled-Synth-newbie-help-request
