/*
 * 4 Ribbon Midi Controller
 * hello@jonathangrover.com
 * Created: 5/27/2022
 * Updated: 6/29/22
*/

#include "SparkFun_Qwiic_Twist_Arduino_Library.h";
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MIDI.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define MIN_PRESSURE 0
#define MAX_PRESSURE 968
#define MIN_MIDI_VELOCITY 0
#define MAX_MIDI_VELOCITY 127
#define MIN_POSITION 0
#define MAX_POSITION 1023

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
TWIST twist;

using Transport = MIDI_NAMESPACE::SerialMIDI<SoftwareSerial>;
int rxPin = 8;
int txPin = 9;
SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);
Transport serialMIDI(mySerial);
MIDI_NAMESPACE::MidiInterface<Transport> MIDI((Transport&)serialMIDI);

// const byte twistInterruptPin = 4;

const int keys = 25; // 0 to 24
const int steps = 56; // 56 notes across 8 octives
int note[keys][steps] = {
      {24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79},                  // Chromatic
      {24,26,28,29,31,33,35,36,38,40,41,43,45,47,48,50,52,53,55,57,59,60,62,64,65,67,69,71,72,74,76,77,79,81,83,84,86,88,89,91,93,95,96,98,100,101,103,105,107,108,110,112,113,115,117,119},      // C maj 
      {24,26,27,29,31,32,34,36,38,39,41,43,44,46,48,50,51,53,55,56,58,60,62,63,65,67,68,70,72,74,75,77,79,80,82,84,86,87,89,91,92,94,96,98,99,101,103,104,106,108,110,111,113,115,116,118},       // C min
      {25,27,29,30,32,34,36,37,39,41,42,44,46,48,49,51,53,54,56,58,60,61,63,65,66,68,70,72,73,75,77,78,80,82,84,85,87,89,90,92,94,96,97,99,101,102,104,106,108,109,111,113,114,116,118,120},      // C# maj
      {25,27,28,30,32,33,35,37,39,40,42,44,45,47,49,51,52,54,56,57,59,61,63,64,66,68,69,71,73,75,76,78,80,81,83,85,87,88,90,92,93,95,97,99,100,102,104,105,107,109,111,112,114,116,117,119},      // C# min 
      {26,28,30,31,33,35,37,38,40,42,43,45,47,49,50,52,54,55,57,59,61,62,64,66,67,69,71,73,74,76,78,79,81,83,85,86,88,90,91,93,95,97,98,100,102,103,105,107,109,110,112,114,115,117,119,121},     // D maj
      {26,28,29,31,33,34,36,38,40,41,43,45,46,48,50,52,53,55,57,58,60,62,64,65,67,69,70,72,74,76,77,79,81,82,84,86,88,89,91,93,94,96,98,100,101,103,105,106,108,110,112,113,115,117,118,120},     // D min
      {27,29,31,32,34,36,38,39,41,43,44,46,48,50,51,53,55,56,58,60,62,63,65,67,68,70,72,74,75,77,79,80,82,84,86,87,89,91,92,94,96,98,99,101,103,104,106,108,110,111,113,115,116,118,120,122},     // D# maj
      {27,29,30,32,34,35,37,39,41,42,44,46,47,49,51,53,54,56,58,59,61,63,65,66,68,70,71,73,75,77,78,80,82,83,85,87,89,90,92,94,95,97,99,101,102,104,106,107,109,111,113,114,116,118,119,121},     // D# min
      {28,30,32,33,35,37,39,40,42,44,45,47,49,51,52,54,56,57,59,61,63,64,66,68,69,71,73,75,76,78,80,81,83,85,87,88,90,92,93,95,97,99,100,102,104,105,107,109,111,112,114,116,117,119,121,123},    // E maj
      {28,30,31,33,35,36,38,40,42,43,45,47,48,50,52,54,55,57,59,60,62,64,66,67,69,71,72,74,76,78,79,81,83,84,86,88,90,91,93,95,96,98,100,102,103,105,107,108,110,112,114,115,117,119,120,122},    // E min
      {29,31,33,34,36,38,40,41,43,45,46,48,50,52,53,55,57,58,60,62,64,65,67,69,70,72,74,76,77,79,81,82,84,86,88,89,91,93,94,96,98,100,101,103,105,106,108,110,112,113,115,117,118,120,122,124},   // F maj
      {29,31,32,34,36,37,39,41,43,44,46,48,49,51,53,55,56,58,60,61,63,65,67,68,70,72,73,75,77,79,80,82,84,85,87,89,91,92,94,96,97,99,101,103,104,106,108,109,111,113,115,116,118,120,121,123},    // F min
      {30,32,34,35,37,39,41,42,44,46,47,49,51,53,54,56,58,59,61,63,65,66,68,70,71,73,75,77,78,80,82,83,85,87,89,90,92,94,95,97,99,101,102,104,106,107,109,111,113,114,116,118,119,121,123,125},   // F# maj
      {30,32,33,35,37,38,40,42,44,45,47,49,50,52,54,56,57,59,61,62,64,66,68,69,71,73,74,76,78,80,81,83,85,86,88,90,92,93,95,97,98,100,102,104,105,107,109,110,112,114,116,117,119,121,122,124},   // F# min
      {31,33,35,36,38,40,42,43,45,47,48,50,52,54,55,57,59,60,62,64,66,67,69,71,72,74,76,78,79,81,83,84,86,88,90,91,93,95,96,98,100,102,103,105,107,108,110,112,114,115,117,119,120,122,124,126},  // G maj
      {31,33,34,36,38,39,41,43,45,46,48,50,51,53,55,57,58,60,62,63,65,67,69,70,72,74,75,77,79,81,82,84,86,87,89,91,93,94,96,98,99,101,103,105,106,108,110,111,113,115,117,118,120,122,123,125},   // G min
      {32,34,36,37,39,41,43,44,46,48,49,51,53,55,56,58,60,61,63,65,67,68,70,72,73,75,77,79,80,82,84,85,87,89,91,92,94,96,97,99,101,103,104,106,108,109,111,113,115,116,118,120,121,123,125,127},  // G# maj
      {32,34,35,37,39,40,42,44,46,47,49,51,52,54,56,58,59,61,63,64,66,68,70,71,73,75,76,78,80,82,83,85,87,88,90,92,94,95,97,99,100,102,104,106,107,109,111,112,114,116,118,119,121,123,124,126},  // G# min
      {33,35,37,38,40,42,44,45,47,49,50,52,54,56,57,59,61,62,64,66,68,69,71,73,74,76,78,80,81,83,85,86,88,90,92,93,95,97,98,100,102,104,105,107,109,110,112,114,116,117,119,121,122,124,126,32},  // A maj
      {33,35,36,38,40,41,43,45,47,48,50,52,53,55,57,59,60,62,64,65,67,69,71,72,74,76,77,79,81,83,84,86,88,89,91,93,95,96,98,100,101,103,105,107,108,110,112,113,115,117,119,120,122,124,125,127}, // A min
      {34,36,38,39,41,43,45,46,48,50,51,53,55,57,58,60,62,63,65,67,69,70,72,74,75,77,79,81,82,84,86,87,89,91,93,94,96,98,99,101,103,105,106,108,110,111,113,115,117,118,120,122,123,125,127,33},  // A# maj
      {34,36,37,39,41,42,44,46,48,49,51,53,54,56,58,60,61,63,65,66,68,70,72,73,75,77,78,80,82,84,85,87,89,90,92,94,96,97,99,101,102,104,106,108,109,111,113,114,116,118,120,121,123,125,126,32},  // A# min
      {35,37,39,40,42,44,46,47,49,51,52,54,56,58,59,61,63,64,66,68,70,71,73,75,76,78,80,82,83,85,87,88,90,92,94,95,97,99,100,102,104,106,107,109,111,112,114,116,118,119,121,123,124,126,32,34},  // B maj
      {35,37,38,40,42,43,45,47,49,50,52,54,55,57,59,61,62,64,66,67,69,71,73,74,76,78,79,81,83,85,86,88,90,91,93,95,97,98,100,102,103,105,107,109,110,112,114,115,117,119,121,122,124,126,127,33}  // B min
    };
int key_pos = 0; // store key position
int twist_pos = 0; // store the knob twist position
int prev_twist_pos = 0; // keep track of previous knob twist value
int min_key = 0; // lowest index
int max_key = keys - 1; // highest index
int sel_key = 0;
int midi_ch = 1;
int vel_threshhold = 5;
String key_label = "";
String note_label = "";
int vel = 0; // velocity label for screen
int curr_note_1 = 0; // current note for ribbon 1
int prev_note_1 = 0; // previous note for ribbon 1
int curr_note_2 = 0; // current note for ribbon 2
int prev_note_2 = 0; // previous note for ribbon 2
int curr_note_3 = 0; // current note for ribbon 3
int prev_note_3 = 0; // previous note for ribbon 3
int curr_note_4 = 0; // current note for ribbon 4
int prev_note_4 = 0; // previous note for ribbon 4

void setup() {
  Serial.begin(9600);
  
  // pinMode(twistInterruptPin, INPUT_PULLUP);
  
  if(twist.begin() == false)
  {
    Serial.println("Twist does not appear to be connected. Please check wiring. Freezing...");
    while(1);
  }
  // twist.setIntTimeout(500);
  prev_twist_pos = twist.getCount();
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();      // Clear the buffer
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void knob() {
  twist_pos = twist.getCount();
  if (twist_pos > prev_twist_pos) {
    if (key_pos < max_key) {
      key_pos = key_pos + 1;
    } else {
      key_pos = min_key;
    }
  }
  else if (twist_pos < prev_twist_pos) {
    if (key_pos > min_key) {
      key_pos = key_pos - 1;
    } else {
      key_pos = max_key;
    }
  }
  prev_twist_pos = twist_pos;
  
//  if (digitalRead(twistInterruptPin) == LOW) {
    if(twist.isClicked()) {
      sel_key = key_pos;
//      Serial.println("Clicked");
      delay(100);
    }
//    if(twist.isPressed()) {
//      Serial.println("Pressed");
//    }
//    twist.clearInterrupts();
//  }
}

void screen() {
  switch (key_pos) {
    case 0:  key_label = "Chromatic"; break;
    case 1:  key_label = "C major"; break;
    case 2:  key_label = "C minor"; break;
    case 3:  key_label = "C# major"; break;
    case 4:  key_label = "C# minor"; break;
    case 5:  key_label = "D major"; break;
    case 6:  key_label = "D minor"; break;
    case 7:  key_label = "D# major"; break;
    case 8:  key_label = "D# minor"; break;
    case 9:  key_label = "E major"; break;
    case 10: key_label = "E minor"; break;
    case 11: key_label = "F major"; break;
    case 12: key_label = "F minor"; break;
    case 13: key_label = "F# major"; break;
    case 14: key_label = "F# minor"; break;
    case 15: key_label = "G major"; break;
    case 16: key_label = "G minor"; break;
    case 17: key_label = "G# major"; break;
    case 18: key_label = "G# minor"; break;
    case 19: key_label = "A major"; break;
    case 20: key_label = "A minor"; break;
    case 21: key_label = "A# major"; break;
    case 22: key_label = "A# minor"; break;
    case 23: key_label = "B major"; break;
    case 24: key_label = "B minor"; break;
    default: key_label = ""; break;
  }
  display.clearDisplay();      // Clear the buffer
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.println(key_label);  // Print the key label
  if (vel > 0) {
    display.println((String)note_label+" "+vel);  // Print the key label
  } else {
    display.println("");       // clear 2nd line
  }
  display.display();           // display new information
}

void displayNote(int n_val) {
  switch (n_val) {
    case 0:  note_label = ""; break;
    case 24: note_label = "C1"; break;
    case 25: note_label = "C#1"; break;
    case 26: note_label = "D1"; break;
    case 27: note_label = "D#1"; break;
    case 28: note_label = "E1"; break;
    case 29: note_label = "F1"; break;
    case 30: note_label = "F#1"; break;
    case 31: note_label = "G1"; break;
    case 32: note_label = "G#1"; break;
    case 33: note_label = "A1"; break;
    case 34: note_label = "A#1"; break;
    case 35: note_label = "B1"; break;
    case 36: note_label = "C2"; break;
    case 37: note_label = "C#2"; break;
    case 38: note_label = "D2"; break;
    case 39: note_label = "D#2"; break;
    case 40: note_label = "E2"; break;
    case 41: note_label = "F2"; break;
    case 42: note_label = "F#2"; break;
    case 43: note_label = "G2"; break;
    case 44: note_label = "G#2"; break;
    case 45: note_label = "A2"; break;
    case 46: note_label = "A#2"; break;
    case 47: note_label = "B2"; break;
    case 48: note_label = "C3"; break;
    case 49: note_label = "C#3"; break;
    case 50: note_label = "D3"; break;
    case 51: note_label = "D#3"; break;
    case 52: note_label = "E3"; break;
    case 53: note_label = "F3"; break;
    case 54: note_label = "F#3"; break;
    case 55: note_label = "G3"; break;
    case 56: note_label = "G#3"; break;
    case 57: note_label = "A3"; break;
    case 58: note_label = "A#3"; break;
    case 59: note_label = "B3"; break;
    case 60: note_label = "C4"; break;
    case 61: note_label = "C#4"; break;
    case 62: note_label = "D4"; break;
    case 63: note_label = "D#4"; break;
    case 64: note_label = "E4"; break;
    case 65: note_label = "F4"; break;
    case 66: note_label = "F#4"; break;
    case 67: note_label = "G4"; break;
    case 68: note_label = "G#4"; break;
    case 69: note_label = "A4"; break;
    case 70: note_label = "A#4"; break;
    case 71: note_label = "B4"; break;
    case 72: note_label = "C5"; break;
    case 73: note_label = "C#5"; break;
    case 74: note_label = "D5"; break;
    case 75: note_label = "D#5"; break;
    case 76: note_label = "E5"; break;
    case 77: note_label = "F5"; break;
    case 78: note_label = "F#5"; break;
    case 79: note_label = "G5"; break;
    case 80: note_label = "G#5"; break;
    case 81: note_label = "A5"; break;
    case 82: note_label = "A#5"; break;
    case 83: note_label = "B5"; break;
    case 84: note_label = "C6"; break;
    case 85: note_label = "C#6"; break;
    case 86: note_label = "D6"; break;
    case 87: note_label = "D#6"; break;
    case 88: note_label = "E6"; break;
    case 89: note_label = "F6"; break;
    case 90: note_label = "F#6"; break;
    case 91: note_label = "G6"; break;
    case 92: note_label = "G#6"; break;
    case 93: note_label = "A6"; break;
    case 94: note_label = "A#6"; break;
    case 95: note_label = "B6"; break;
    case 96: note_label = "C7"; break;
    case 97: note_label = "C#7"; break;
    case 98: note_label = "D7"; break;
    case 99: note_label = "D#7"; break;
    case 100: note_label = "E7"; break;
    case 101: note_label = "F7"; break;
    case 102: note_label = "F#7"; break;
    case 103: note_label = "G7"; break;
    case 104: note_label = "G#7"; break;
    case 105: note_label = "A7"; break;
    case 106: note_label = "A#7"; break;
    case 107: note_label = "B7"; break;
    case 108: note_label = "C8"; break;
    case 109: note_label = "C#8"; break;
    case 110: note_label = "D8"; break;
    case 111: note_label = "D#8"; break;
    case 112: note_label = "E8"; break;
    case 113: note_label = "F8"; break;
    case 114: note_label = "F#8"; break;
    case 115: note_label = "G8"; break;
    case 116: note_label = "G#8"; break;
    case 117: note_label = "A8"; break;
    case 118: note_label = "A#8"; break;
    case 119: note_label = "B8"; break;
    case 120: note_label = "C9"; break;
    case 121: note_label = "C#9"; break;
    case 122: note_label = "D9"; break;
    case 123: note_label = "D#9"; break;
    case 124: note_label = "E9"; break;
    case 125: note_label = "F9"; break;
    case 126: note_label = "F#9"; break;
    case 127: note_label = "G9"; break;
    default: note_label = ""; break;
  }
}

void ribbon() {
  
  // Ribbon 1
  int pres_ribbon_1 = analogRead(A0); 
  int posi_ribbon_1 = analogRead(A1) - 10; // correct drift on this ribbon.
  
  int velocity_1 = map(pres_ribbon_1, MIN_PRESSURE, MAX_PRESSURE, MIN_MIDI_VELOCITY, MAX_MIDI_VELOCITY);
  int note_index_1 = map(posi_ribbon_1, MIN_POSITION, MAX_POSITION, 0, 14);

  if (velocity_1 > vel_threshhold) {
    int note_val_1 = note[sel_key][note_index_1];
    curr_note_1 = note_val_1;
    if (curr_note_1 != prev_note_1) {
      MIDI.sendNoteOn(note_val_1, velocity_1, midi_ch); // Note On
      displayNote(note_val_1);
    }
    prev_note_1 = curr_note_1;
  }
  else {
    for (int i = 0; i < 14; i++) {
      int note_val = note[sel_key][i];
      MIDI.sendNoteOff(note_val, 0, midi_ch); // Note Off
    }
    prev_note_1 = 0;
  }

  // Ribbon 2
  int pres_ribbon_2 = analogRead(A2); 
  int posi_ribbon_2 = analogRead(A3) + 24; // correct drift on this ribbon. 
  int velocity_2 = map(pres_ribbon_2, MIN_PRESSURE, MAX_PRESSURE, MIN_MIDI_VELOCITY, MAX_MIDI_VELOCITY);
  int note_index_2 = map(posi_ribbon_2, MIN_POSITION, MAX_POSITION, 14, 28);

  if (velocity_2 > vel_threshhold) {
    int note_val_2 = note[sel_key][note_index_2];
    curr_note_2 = note_val_2;
    if (curr_note_2 != prev_note_2) {
      MIDI.sendNoteOn(note_val_2, velocity_2, midi_ch); // Note On
      displayNote(note_val_2);
    }
    prev_note_2 = curr_note_2;
  }
  else {
    for (int i = 14; i < 28; i++) {
      int note_val_2 = note[sel_key][i];
      MIDI.sendNoteOff(note_val_2, 0, midi_ch); // Note Off
    }
    prev_note_2 = 0;
  }

  // Ribbon 3
  int pres_ribbon_3 = analogRead(A4);
  int posi_ribbon_3 = analogRead(A5);  
  int velocity_3 = map(pres_ribbon_3, MIN_PRESSURE, MAX_PRESSURE, MIN_MIDI_VELOCITY, MAX_MIDI_VELOCITY);
  int note_index_3 = map(posi_ribbon_3, MIN_POSITION, MAX_POSITION, 28, 42);

  if (velocity_3 > vel_threshhold) {
    int note_val_3 = note[sel_key][note_index_3];
    curr_note_3 = note_val_3;
    if (curr_note_3 != prev_note_3) {
      MIDI.sendNoteOn(note_val_3, velocity_3, midi_ch); // Note On
      displayNote(note_val_3);
    }
    prev_note_3 = curr_note_3;
  }
  else {
    for (int i = 28; i < 42; i++) {
      int note_val_3 = note[sel_key][i];
      MIDI.sendNoteOff(note_val_3, 0, midi_ch); // Note Off
    }
    prev_note_3 = 0;
  }

   // Ribbon 4
  int pres_ribbon_4 = analogRead(A8); 
  int posi_ribbon_4 = analogRead(A9);
  int velocity_4 = map(pres_ribbon_4, MIN_PRESSURE, MAX_PRESSURE, MIN_MIDI_VELOCITY, MAX_MIDI_VELOCITY);
  int note_index_4 = map(posi_ribbon_4, MIN_POSITION, MAX_POSITION, 42, 56);

  if (velocity_4 > vel_threshhold) {
    int note_val_4 = note[sel_key][note_index_4];
    curr_note_4 = note_val_4;
    if (curr_note_4 != prev_note_4) {
      MIDI.sendNoteOn(note_val_4, velocity_4, midi_ch); // Note On
      displayNote(note_val_4);
    }
    prev_note_4 = curr_note_4;
  }
  else {
    for (int i = 42; i < 56; i++) {
      int note_val_4 = note[sel_key][i];
      MIDI.sendNoteOff(note_val_4, 0, midi_ch); // Note Off
    }
    prev_note_4 = 0;
  }

  // update velocity label for screen
  if (velocity_1 > vel_threshhold) {
    vel = velocity_1;
  } else if (velocity_2 > vel_threshhold) {
    vel = velocity_2;
  } else if (velocity_3 > vel_threshhold) {
    vel = velocity_3;
  } else if (velocity_4 > vel_threshhold) {
    vel = velocity_4;
  } else {
    vel = 0;
  }
  
}

void loop() {
  knob();
  screen();
  ribbon();
}
