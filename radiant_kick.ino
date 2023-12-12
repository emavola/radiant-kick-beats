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
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
}


// loop() function -- runs repeatedly as long as board is on ---------------
int res = 0;
int blue = 0;
float red = 255;
int green = 255;
int oldAnalog = 0;
int TRESHOLD = 6;
void loop() {
  red = random(0, 255);
  blue = random(0, 255);
  green = random(0, 255);
  if (abs(analogRead(A0) - oldAnalog) > TRESHOLD) {
    trigger(30, 100, 100);
  }
  Serial.println(abs(analogRead(A0) - oldAnalog));
  oldAnalog = analogRead(A0);
  delay(10);
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
