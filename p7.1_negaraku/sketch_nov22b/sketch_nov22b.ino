/* Play Melody
 * -----------
 *
 * Program to play a simple melody
 *
 * Tones are created by quickly pulsing a speaker on and off 
 *   using PWM, to create signature frequencies.
 *
 * Each note has a frequency, created by varying the period of 
 *  vibration, measured in microseconds. We'll use pulse-width
 *  modulation (PWM) to create that vibration.

 * We calculate the pulse-width to be half the period; we pulse 
 *  the speaker HIGH for 'pulse-width' microseconds, then LOW 
 *  for 'pulse-width' microseconds.
 *  This pulsing creates a vibration of the desired frequency.
 *
 * (cleft) 2005 D. Cuartielles for K3
 * Refactoring and comments 2006 clay.shirky@nyu.edu
 * See NOTES in comments at end for possible improvements
 */

// TONES  ==========================================
// Start by defining the relationship between 
//       note, period, &  frequency. 
// #define  c     3830    // 261 Hz 
// #define  d     3400    // 294 Hz 
// #define  e     3038    // 329 Hz 
// #define  f     2864    // 349 Hz 
// #define  g     2550    // 392 Hz 
// #define  a     2272    // 440 Hz 
// #define  b     2028    // 493 Hz 
// #define  C     1912    // 523 Hz 
// Define a special note, 'R', to represent a rest
// #define  R     0

#include "pitches.h"

// SETUP ============================================
// Set up speaker on a PWM pin (digital 9, 10 or 11)
int speakerOut = 9;
// Do we want debugging on serial out? 1 for yes, 0 for no
int DEBUG = 0;

void setup() { 
  pinMode(speakerOut, OUTPUT);
  if (DEBUG) { 
    //Serial.begin(9600); // Set serial out if we want debugging
  }
  Serial.begin(9600); // Set serial out if we want debugging
}

// MELODY and TIMING  =======================================
//  melody[] is an array of notes, accompanied by beats[], 
//  which sets each note's relative length (higher #, longer note) 

// Negaraku Melody

int melody[] = {
  NOTE_D7, NOTE_D7, NOTE_F6, NOTE_A6, 0,
  NOTE_A6, NOTE_A6, NOTE_C6, NOTE_E6, NOTE_F6, NOTE_AS6, NOTE_A6, NOTE_G6, 0,
  NOTE_C7, NOTE_AS6, NOTE_GS6, NOTE_FS6, 0,
  NOTE_E6, NOTE_DS6, NOTE_B5, NOTE_C6, NOTE_D6, NOTE_DS6, 0,
  NOTE_B6, NOTE_A6, NOTE_G6, NOTE_F6, 0,
  NOTE_A6, NOTE_G6, NOTE_A6, NOTE_G6, NOTE_F6, NOTE_E6, 0,
  NOTE_D7, NOTE_B6, NOTE_G6, NOTE_A6, 0,
  NOTE_C7, NOTE_A6, NOTE_AS6, NOTE_B6, NOTE_D7, NOTE_E7 
};
int beats[]  = {
  64, 64, 64, 128, 16,
  64, 64, 64, 64, 64, 108, 32, 128, 16,
  64, 64, 64, 128, 16,
  64, 64, 64, 108, 32, 138, 16,
  64, 64, 64, 128, 16,
  64, 108, 32, 64, 64, 128, 16,
  64, 64, 64, 128, 16,
  64, 64, 64, 108, 32, 148
  
};

char* lyrics[] = {
  "ne", "ga", "ra", "ku", "\n",
  "ta", "nah ", "tum", "pah", "nya ", "da", "rah", "ku", "\n",
  "rak", "yat ", "hi", "dup", "\n",
  "ber", "sa", "tu ", "dan ", "ma", "ju ", "\n",
  "rah", "mat ", "bah", "gia", "\n",
  "tu", "han ", "kur", "ni", "a", "kan", "\n",
  "ra", "ja ", "ki", "ta", "\n",
  "se", "la", "mat ", "ber", "takh", "ta"
};

int MAX_COUNT = sizeof(melody) / 2; // Melody length, for looping.

// Set overall tempo
long tempo = 10000;
// Set length of pause between notes
int pause = 1000;
// Loop variable to increase Rest length
int rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int tone_ = 0;
int beat = 0;
long duration  = 0;

// PLAY TONE  ==============================================
// Pulse the speaker to play a tone for a particular duration
void playTone() {
  long elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has 
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {

      digitalWrite(speakerOut,HIGH);
      delayMicroseconds(tone_ / 2);

      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(tone_ / 2);

      // Keep track of how long we pulsed
      elapsed_time += (tone_);
    } 
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }                                 
}

// LET THE WILD RUMPUS BEGIN =============================
void loop() {
  // Set up a counter to pull from melody[] and beats[]
  Serial.print(lyrics[0]);
  for (int i=0; i<MAX_COUNT; i++) {
    tone_ = melody[i];
    beat = beats[i];

    duration = beat * tempo; // Set up timing

    playTone(); 
    // A pause between notes...
    delayMicroseconds(pause);

    if (DEBUG) { // If debugging, report loop, tone, beat, and duration
      Serial.print(i);
      Serial.print(":");
      Serial.print(beat);
      Serial.print(" ");    
      Serial.print(tone_);
      Serial.print(" ");
      Serial.println(duration);
    }
    
    Serial.print(lyrics[i+1]);
  }
  exit(0);  
}

/*
 * NOTES
 * The program purports to hold a tone for 'duration' microseconds.
 *  Lies lies lies! It holds for at least 'duration' microseconds, _plus_
 *  any overhead created by incremeting elapsed_time (could be in excess of 
 *  3K microseconds) _plus_ overhead of looping and two digitalWrites()
 *  
 * As a result, a tone of 'duration' plays much more slowly than a rest
 *  of 'duration.' rest_count creates a loop variable to bring 'rest' beats 
 *  in line with 'tone' beats of the same length. 
 * 
 * rest_count will be affected by chip architecture and speed, as well as 
 *  overhead from any program mods. Past behavior is no guarantee of future 
 *  performance. Your mileage may vary. Light fuse and get away.
 *  
 * This could use a number of enhancements:
 * ADD code to let the programmer specify how many times the melody should
 *     loop before stopping
 * ADD another octave
 * MOVE tempo, pause, and rest_count to #define statements
 * RE-WRITE to include volume, using analogWrite, as with the second program at
 *          http://www.arduino.cc/en/Tutorial/PlayMelody
 * ADD code to make the tempo settable by pot or other input device
 * ADD code to take tempo or volume settable by serial communication 
 *          (Requires 0005 or higher.)
 * ADD code to create a tone offset (higer or lower) through pot etc
 * REPLACE random melody with opening bars to 'Smoke on the Water'
 */
