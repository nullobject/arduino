/**
 * RGB matrix controller.
 * Copyright 2015, Josh Bassett.
 */

#include <SPI.h>

#define BLACK   0x00
#define RED     0xE0
#define GREEN   0x1C
#define BLUE    0x03
#define YELLOW  0xE0|GREEN
#define ORANGE  0xFC
#define MAGENTA 0xE0|BLUE
#define TEAL    BLUE|GREEN
#define WHITE   (0xE0|GREEN|BLUE)-0xA0

#define SLAVESELECT 2

uint8_t redStroke[] = {
  0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,
  0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
  0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
  0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
  0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
  0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
  0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
  0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0
};

uint8_t greenStroke[] = {
  0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C,
  0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C,
  0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C,
  0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C,
  0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C,
  0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C,
  0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C,
  0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C
};

const uint8_t numRows = 8;
const uint8_t numCols = 8;
const uint8_t bufferSize = numRows * numCols;
const uint8_t numSequences = 4;

// The buffer which holds the LED colour values.
uint8_t ledBuffer[bufferSize];

uint8_t sequence, step;

void (*sequences[numSequences])();

void setup(){
  randomSeed(analogRead(0));
  
  setupSerial();
  setupMatrix();
  bufferClear();
  bufferWrite();
  
  sequences[0] = &redFillSequence;
  sequences[1] = &greenFillSequence;
  sequences[2] = &redStrokeSequence;
  sequences[3] = &greenStrokeSequence;
}

void setupSerial() {
  // Initialize the serial port.
  Serial.begin(9600);
}

void setupMatrix() {
  // Initialize the SPI controller.
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);

  // Configure the slave select pin.
  pinMode(SLAVESELECT, OUTPUT);

  // Make sure the matrix is deactivated.
  digitalWrite(SLAVESELECT, HIGH);
}

//void loop() {
//  for (uint8_t i = 0; i < 64; i++) {
//    ledBuffer[i] = random(255);
//  }
//  
//  bufferWrite();
//  delay(80);
//}

void loop() {
  handleInput();
  
  // Step the sequence.
  (*sequences[sequence])();
  
  // Increment the current step.
  step++;
}

void handleInput() {
  while (Serial.available() > 0) {
    int value = Serial.parseInt();
    if (Serial.read() == '\n') {
      changeSequence(value);
    }
  }
}

void changeSequence(uint8_t value) {
  sequence = constrain(value, 0, numSequences - 1);
  step = 0; 
  bufferClear();
}

void redFillSequence() {
  memset(ledBuffer, RED, bufferSize);
  bufferWrite();
}

void greenFillSequence() {
  memset(ledBuffer, GREEN, bufferSize);
  bufferWrite();
}

void redStrokeSequence() {
  memcpy(ledBuffer, redStroke, bufferSize);
  bufferWrite();
}

void greenStrokeSequence() {
  memcpy(ledBuffer, greenStroke, bufferSize);
  bufferWrite();
}

/**
 * Clear the colour buffer.
 */
void bufferClear() {
  memset(ledBuffer, BLACK, bufferSize);
}

/**
 * Write the colour buffer to the matrix.
 */
void bufferWrite() {
  // Activate the matrix.
  digitalWrite(SLAVESELECT, LOW);
  
  // Allow some time for the matrix to become active.
  delayMicroseconds(500);

  // Send the buffer to the matrix.
  for (uint8_t led = 0; led < 64; led++) {
    SPI.transfer(ledBuffer[led]);
  }

  // Allow some time for the serial data to be sent.
  delayMicroseconds(500);

  // Deactivate the matrix.
  digitalWrite(SLAVESELECT, HIGH);
}
