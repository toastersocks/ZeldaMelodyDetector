	//
	// melodyDetector-Xcode
	//
	// Detect melodies
	// Developed with [embedXcode](http://embedXcode.weebly.com)
	//
	// Author	 	James Pamplona
	// 				James Pamplona
	//
	// Date			8/8/14 2:16 PM
	// Version		<#version#>
	//
	// Copyright	© James Pamplona, 2014
	// License		<#license#>
	//
	// See			ReadMe.txt for references
	//

	// Core library for code-sense
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(MICRODUINO) // Microduino specific
#include "Arduino.h"
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad MSP430, Stellaris and Tiva, Experimeter Board FR5739 specific
#include "Energia.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif

	// Include application, user and local libraries
#include "LocalLibrary.h"


	// Prototypes


	// Define variables and constants
	//
	// Brief	Name of the LED
	// Details	Each board has a LED but connected to a different pin
	//
//uint8_t myLED;


	//
	// Brief	Setup
	// Details	Define the pin the LED is connected to
	//
	// Add setup code
#include "pitches.h"

#define SONG_SIZE 6
#define PRECISION_POT 5

byte MATCHING_PRECISION = 1;

	// 0 absolute pitches
	// 1 pitch class
	// 2 relative pitch class
	// 3 reletive absolute pitch
enum matchingMode_t {kMatchAbsolute, kMatchPitchClass, kMatchRelativePitchClass, kMatchRelativeAbsolute};
boolean songsMatch();

template <typename T>void printArrayOfSize(T array[], byte arraySize);
template<typename SongType>boolean doSongsofLengthMatchWithPrecision(SongType song1, SongType song2, byte songSize, byte precision);
void shiftLeftArrayOfSizeWithData( byte array[], byte arraySize, byte newData);
byte getNoteWithMode(matchingMode_t mode);

matchingMode_t matchingMode = kMatchRelativeAbsolute;


byte songs[][SONG_SIZE] = {ZELDAS_LULLABY, EPONAS_SONG, SARIAS_SONG, SONG_OF_TIME, SUNS_SONG, SONG_OF_STORMS};

byte * song = {songs[3]}; // Song of Time
signed char pitchDifferences[5];


byte incomingNotes[] = {0, 0, 0, 0, 0, 0};
	//byte incomingNote = 0;
	//byte oldIncomingNote = 0;

size_t sizeofSongArrays = SONG_SIZE; //size of arrays


void setup() {
	while (!Serial && !Serial1) ;	//wait for serial ports to be ready
	Serial.begin(9600);
	Serial1.begin(9600);
	Serial1.println("Serial ready...");
	//////////////////////////////////////
	pinMode(5, INPUT);

	
	for (int i = 0; i < SONG_SIZE - 1; i++) {
		pitchDifferences[i] = song[i] - song[i+1];
	}


}


void loop() {
	static int precisionPotVal;
	precisionPotVal = analogRead(PRECISION_POT);
	MATCHING_PRECISION = map(precisionPotVal, 0, 1020, 0, 3);
//	Serial1.print("Pot value is: "); Serial1.println(precisionPotVal);
	
	if (Serial.available() > 0) {
		
		byte note = getNoteWithMode(kMatchPitchClass);
		Serial1.print("Incoming note: ");Serial1.println(note);
		shiftLeftArrayOfSizeWithData(incomingNotes, SONG_SIZE, note);
		
//		Serial1.print("Looking for:    "); printArrayOfSize(song, SONG_SIZE);
//		Serial1.print("Song in buffer: "); printArrayOfSize(incomingNotes, SONG_SIZE);
		
		if (songsMatch()) { //check if the stream of incoming notes matches the song we're looking for
			Serial1.println("HEARD SONG!");
		}
		
	}
}

void shiftLeftArrayOfSizeWithData( byte array[], byte arraySize, byte newData) {
	memmove(&array[0], &array[1], ( arraySize * sizeof(array[0]) ) - sizeof(*array)); // shift values in the incoming notes array
	array[arraySize - 1] = newData; // put new data on the end
}

template <typename T>void printArrayOfSize(T array[], byte arraySize ) {
	for (byte i = 0; i < arraySize; i++) {
		Serial1.print(array[i]); Serial1.print(" ");
	}
	Serial1.println();
	
}

	///Gets the note from Serial and returns it in appropriate form. Right now only working with pitch classes i.e. 0 - 11 representing C - B regardless of octave
byte getNoteWithMode(matchingMode_t mode) {
	static byte incomingNote;
	static byte oldIncomingNote;
	
	if (Serial.available() > 0) {
		
		incomingNote = Serial.read();
//		Serial1.print("raw incoming note: ")Serial1.println(incomingNote);
		
		if (incomingNote != oldIncomingNote) {
			static byte secondIncomingNote;
			static byte noteDifference;

			switch (mode) {
				case kMatchAbsolute:
					return incomingNote;
					break;
					
				case kMatchPitchClass:
					return incomingNote % 12;
					break;
					
				case kMatchRelativeAbsolute:
					while (!(Serial.available() > 0)) {
							// wait for note from Serial
					}
					secondIncomingNote = Serial.read();
//					static byte noteDifference;
					noteDifference = incomingNote - secondIncomingNote;
					incomingNote = secondIncomingNote;
					return noteDifference;
					break;
					
				case kMatchRelativePitchClass:
					while (!(Serial.available() > 0)) {
							// wait for note from Serial
					}
					secondIncomingNote = Serial.read();
//					static byte noteDifference;
					noteDifference = (incomingNote % 12) - (secondIncomingNote % 12);
					incomingNote = secondIncomingNote;
					return noteDifference;
					break;
					
				default:
					Serial.println("ERROR: invalid mode");
					break;
			}
			oldIncomingNote = incomingNote;
		}
	}
}
boolean songsMatch() {
	
	switch (matchingMode) {
			
		case kMatchPitchClass:
			
			return doSongsofLengthMatchWithPrecision(song, incomingNotes, SONG_SIZE, MATCHING_PRECISION);
//				return ! ( memcmp(song, incomingNotes, sizeofSongArrays * sizeof(byte)) ); //check if the stream of incoming notes matches the song we're looking for
			break;
			
		case kMatchRelativePitchClass:
		case kMatchRelativeAbsolute:
			
			static signed char incomingDifferences[5];
			
			for (byte i = 0; i < SONG_SIZE - 1; i++) {
				incomingDifferences[i] = incomingNotes[i] - incomingNotes[i+1];
			}
			Serial1.print("Looking for song: "); printArrayOfSize(song, SONG_SIZE);
			Serial1.print("Song in buffer:   "); printArrayOfSize(incomingNotes, SONG_SIZE);
			
			return doSongsofLengthMatchWithPrecision(pitchDifferences, incomingDifferences, SONG_SIZE - 1, MATCHING_PRECISION);
//			return ! ( memcmp(pitchDifferences, incomingDifferences, (sizeofSongArrays - 1) * sizeof(byte)) ) ;
			break;
			
		default:
			break;
	}
}

template<typename SongType>boolean doSongsofLengthMatchWithPrecision(SongType song1, SongType song2, byte songSize, byte precision) {
	Serial1.print("Matching precision is: ");Serial1.println(precision);
	boolean isMatch = true;
//	Serial1.print("songSize: "); Serial1.println(songSize);
	if (matchingMode == kMatchRelativePitchClass || matchingMode == kMatchRelativeAbsolute) {
		Serial1.print("Looking for difference: "); printArrayOfSize(song1, songSize);
		Serial1.print("Difference in buffer:   "); printArrayOfSize(song2, songSize);
	}
	for (byte i = 0; i < songSize; i++) {
		if ( (abs(song1[i] - song2[i]) > precision) ) { // if the difference in notes exceeds the precision tolerance, the test fails and songs do not match
			isMatch = false;
			break;
		}
	}
	return isMatch;

	
}