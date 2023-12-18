/*
    File: radiant_kick_beats.ino
    Description: Arduino program for the Radiant Kick Beat project,
    which combines sound detection with dynamic LED illumination in response to kicks or hits.
    Authors: Emanuele Volanti
    Date of Creation: 7/12/2023

    This program uses an Adafruit NeoPixel strip for visual output and a sound sensor module connected to analog input A0 to detect percussive events.
    The LED strip responds to kicks by creating vibrant light patterns.
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN    6
#define LED_COUNT 60


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255);
}


// loop() function -- runs repeatedly as long as board is on ---------------
int res = 0;
int blue = 0;
float red = 255;
int green = 255;
int TRESHOLD = 500;



const int BUFFER_SIZE = 6;
int audioBuffer[BUFFER_SIZE];
int bufferIndex = 0;
int prevKick = 0;
const float CODE_ARR[] = {1, 0.5, 0.5, 1, 2, 1};
const float DELTA_ERROR = 0.10;

bool isMenu = false;

void loop() {
  red = random(0, 255);
  blue = random(0, 255);
  green = random(0, 255);

  int a = analogRead(A0);
  
  //Serial.println(audioBuffer);
  if (a > TRESHOLD) {
    int currKick = millis();
    audioBuffer[bufferIndex] = currKick - prevKick;
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
    prevKick = currKick;
    if (checkMenu()){
      Serial.println("MENU");
    }
    trigger(10, 10, 10);
  }
}

bool checkMenu(){
  for (int steps = 0; steps < BUFFER_SIZE; steps++) {
    int index = (bufferIndex + steps) % BUFFER_SIZE;
    float ratio = (float) audioBuffer[index] / audioBuffer[bufferIndex];
    if (ratio < CODE_ARR[steps] - DELTA_ERROR || ratio > CODE_ARR[steps] + DELTA_ERROR) {
      return false;
    }
  }
  //printArray(audioBuffer, 6);
  return true;
}

void trigger(int durationFadeIn, int durationFadeOut, int sustain) {
  fadeIn(durationFadeIn);
  delay(sustain);
  fadeOut(durationFadeOut);
}

void fadeIn(int duration) {
  float del = duration / 60;
  for (int i = strip.numPixels() / 2; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(blue, green, red));
    strip.setPixelColor(strip.numPixels() - i, strip.Color(blue, green, red));
    strip.show();
    delay(del);
  }
}

void fadeOut(int duration) {
  float del = duration / 60;
  for (int i = 0; i <= strip.numPixels() / 2; i++) { // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.setPixelColor(strip.numPixels() - i, strip.Color(0, 0, 0));
    strip.show();
    delay(del);
  }
}

void printArray(int array[], int lunghezza) {
  Serial.print("Array: ");
  for (int i = 0; i < lunghezza; i++) {
    Serial.print(array[i]);
    if (i < lunghezza - 1) {
      Serial.print(", ");
    }
  }
  Serial.println();
}