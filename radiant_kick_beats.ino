/*
    File: radiant_kick_beats.ino
    Description: Arduino program for the Radiant Kick Beat project,
    which combines sound detection with dynamic LED illumination in response to kicks or hits.
    Authors: Emanuele Volanti
    Date of Creation: 7/12/2023

    This program uses an Adafruit NeoPixel strip for visual output and a sound sensor module connected to analog input A0 to detect percussive events.
    The LED strip responds to kicks by creating vibrant light patterns.
*/

#include <TaskScheduler.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN    6
#define LED_COUNT 60


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Callback prototypes 
void preview();
void selectMode();


Task previewTask(1000, TASK_FOREVER, &preview);
Task selectModeTask(2000, 2, &selectMode);
Scheduler runner;

void setup() {
  Serial.begin(9600);
  
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255);
  
  runner.init();
  runner.addTask(previewTask);
  runner.addTask(selectModeTask);
}
// used to select a mode
int selectionKickNum = 0;

const int TRESHOLD = 500;
int mode = 0;
const int MODE_QUANTITY = 3;


const int BUFFER_SIZE = 6;
int audioBuffer[BUFFER_SIZE];
int bufferIndex = 0;
int prevKick = 0;
const float CODE_ARR[] = {1, 0.5, 0.5, 1, 2, 1};
const float DELTA_ERROR = 0.10;

bool isMenu = false;

void loop() {
  runner.execute();

  // Drum kick is hitted
  if (analogRead(A0) > TRESHOLD) {
    // Record kick
    Serial.println("Hitted");
    int currKick = millis();
    audioBuffer[bufferIndex] = currKick - prevKick;
    bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
    prevKick = currKick;
    
    // Kick with the active menu
    if (isMenu){
      Serial.println("Hitted with menu");
      selectionKickNum++;
      
      if (selectModeTask.isEnabled()){
        if (selectionKickNum == 2) {
          mode = (mode + 1) % MODE_QUANTITY;
          selectModeTask.cancel();
          selectionKickNum = 0;
        }
      } else {
        selectModeTask.restart();
      }
      delay(100);
    } else {
      trigger();
    }
    
    if (checkMenu()) {
      Serial.println("Menu triggered");
      isMenu = true;
      fadeInColor('r', 1000);
      fadeOutColor('r', 1000);
      fadeInColor('r', 1000);
      fadeOutColor('r', 1000);
      previewTask.enable();
    }
    
  }
}

// Detect menu pattern and set menu as true
bool checkMenu(){
  for (int steps = 0; steps < BUFFER_SIZE; steps++) {
    int index = (bufferIndex + steps) % BUFFER_SIZE;
    float ratio = (float) audioBuffer[index] / audioBuffer[bufferIndex];
    if (ratio < CODE_ARR[steps] - DELTA_ERROR || ratio > CODE_ARR[steps] + DELTA_ERROR) {
      return false;
    }
  }
  return true;
}

void exitMenu() {
  isMenu = false;
  previewTask.cancel();
  fadeInColor('g', 1000);
  fadeOutColor('g', 1000);
}

void preview() {
  trigger();
}

void selectMode() {
  Serial.println("Select mode");
  if (selectModeTask.isLastIteration()) {
    if (selectionKickNum < 2) {
      exitMenu();
    }
    selectionKickNum = 0;
  }
  
}


void trigger(){
  switch (mode) {
    case 0:
      modeRandomTrigger(10, 10, 10);
      break;
    case 1:
      modeWrongTrigger(100);
      break;
    case 2:
      modeRandomFixedTrigger();
      break;
  }
}


// --- RANDOM MODE ---
void modeRandomTrigger(int durationFadeIn, int durationFadeOut, int sustain) {
  int red = random(0, 255);
  int blue = random(0, 255);
  int green = random(0, 255);
  
  fromCenterIn(durationFadeIn, blue, green, red);
  delay(sustain);
  fromCenterOut(durationFadeOut);
}

void fromCenterIn(int duration, int blue, int green, int red) {
  float del = duration / 60;
  for (int i = strip.numPixels() / 2; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(blue, green, red));
    strip.setPixelColor(strip.numPixels() - i, strip.Color(blue, green, red));
    strip.show();
    delay(del);
  }
}

void fromCenterOut(int duration) {
  float del = duration / 60;
  for (int i = 0; i <= strip.numPixels() / 2; i++) { // For each pixel in strip...
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.setPixelColor(strip.numPixels() - i, strip.Color(0, 0, 0));
    strip.show();
    delay(del);
  }
}

// --- WRONG MODE ---

void modeWrongInit() {
  strip.fill(strip.Color(255, 255, 255));
}

void modeWrongTrigger(int sustain) {
  strip.clear();
  strip.show();
  delay(sustain);
  strip.fill(strip.Color(255, 255, 255));
  strip.show();
}

// --- RANDOM FIXED ---
void modeRandomFixedInit(){
  int red = random(0, 255);
  int blue = random(0, 255);
  int green = random(0, 255);

  strip.fill(strip.Color(red, green, blue));
  strip.show();
}

void modeRandomFixedTrigger() {
  int red = random(0, 255);
  int blue = random(0, 255);
  int green = random(0, 255);

  strip.fill(strip.Color(red, green, blue));
  strip.show();
}


// --- COMMON STRIP PATTERNS ---

void fadeInColor(char color, int duration) {
  for (int x = 0; x <= 255; x = x + 5) {
    switch (color) {
      case 'r':
        strip.fill(strip.Color(x, 0, 0));
        strip.show();
        break;
      case 'g':
        strip.fill(strip.Color(0, x, 0));
        strip.show();
        break;
    }
    delay(duration / (255 / 5));
  }
  
}

void fadeOutColor(char color, int duration) {
  for (int x = 255; x >= 0; x = x - 5) {
    switch (color) {
      case 'r':
        strip.fill(strip.Color(x, 0, 0));
        strip.show();
        break;
      case 'g':
        strip.fill(strip.Color(0, x, 0));
        strip.show();
        break;
    }
    delay(duration / (255 / 5));
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