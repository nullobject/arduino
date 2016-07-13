#include <Adafruit_NeoPixel.h>

const uint8_t numSequences = 4;
const uint8_t pin1 = 3;
const uint8_t pin2 = 4;
const uint8_t pixels = 64;
const uint8_t brightness = 0x10;

void (*sequences[numSequences])();

uint8_t sequence, step;

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(pixels, pin1);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(pixels, pin2);

void setup() {
  setupSerial();
  setupStrips();
  
  sequences[0] = &redFillSequence;
  sequences[1] = &greenFillSequence;
  sequences[2] = &redFadeSequence;
  sequences[3] = &greenFadeSequence;
}

void setupSerial() {
  // Initialize the serial port.
  Serial.begin(9600);
  
  // Wait for the serial port to connect.
//  while (!Serial);

  // Set the initial sequence.
  changeSequence(0);
}

void setupStrips() {
  // Initialize the LED strips.
  strip1.begin();
  strip2.begin();
  
  // Clear the LED strips.
  strip1.show();
  strip2.show();
}

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
  strip1.setBrightness(brightness);
  strip2.setBrightness(brightness);
  
  sequence = constrain(value, 0, numSequences - 1);
  step = 0;
  
  fill(&strip1, 0);
  fill(&strip2, 0);
}

void redFadeSequence() {
  fadeSequence(Wheel(&strip1, 0x40));
}

void greenFadeSequence() {
  fadeSequence(Wheel(&strip1, 0x00));
}

void redFillSequence() {
  fill(&strip1, Wheel(&strip1, 0x40));
  fill(&strip2, Wheel(&strip2, 0x40));
}

void greenFillSequence() {
  fill(&strip1, Wheel(&strip1, 0x00));
  fill(&strip2, Wheel(&strip2, 0x00));
}

void wipeSequence() {
  wipe(&strip1, Wheel(&strip1, (step * 0x40) & 0xff), 50);
  wipe(&strip2, Wheel(&strip2, (step * 0x40) & 0xff), 50);
}

void fadeSequence(uint32_t colour) {
  uint8_t b;
  
  if (step < 0x80) {
    b = (float)step / 0xff * (float)brightness;
  } else {
    b = brightness - ((float)step / 0xff * (float)brightness);
  }
  
  strip1.setBrightness(b);
  strip2.setBrightness(b);
  
  fill(&strip1, colour);
  fill(&strip2, colour);
}

void bounceSequence(uint32_t colour) {
  uint8_t n = (float)step / 0xff * (float)pixels;
  strip1.setPixelColor(n - 1, 0);
  strip1.setPixelColor(n, colour);
  strip1.show();
  strip2.setPixelColor(n - 1, 0);
  strip2.setPixelColor(n, colour);
  strip2.show();
  delay(2);
}

// Fill all the pixels immediately with a colour.
void fill(Adafruit_NeoPixel *strip, uint32_t color) {
  for (uint8_t i = 0; i < pixels; i++) {
    strip->setPixelColor(i, color);
  }
  strip->show();
}

// Fill the pixels one after the other with a colour.
void wipe(Adafruit_NeoPixel *strip, uint32_t color, uint8_t wait) {
  for (uint8_t i = 0; i < pixels; i++) {
    strip->setPixelColor(i, color);
    strip->show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(Adafruit_NeoPixel *strip, uint8_t value) {
  if (value < 85) {
    return strip->Color(value * 3, 255 - value * 3, 0);
  } else if (value < 170) {
    value -= 85;
    return strip->Color(255 - value * 3, 0, value * 3);
  } else {
    value -= 170;
    return strip->Color(0, value * 3, 255 - value * 3);
  }
}
