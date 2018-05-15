#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#define NUM_LEDS 130
#define DATA_PIN 30


CRGB leds[NUM_LEDS];
static unsigned long timers[2] = {0, 0};
static int stage = 0;


void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812, DATA_PIN>(leds, NUM_LEDS); // GRB
  Serial.begin(9600);
}


void on_air() {
  int i = 0;
  int red = 240;  
  static int j = 0; 

  if (stage == 0) {
    // Off
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    timers[0] = millis();
    stage++;
  }

  if (stage == 1 && (millis() - timers[0]) > 200) {

    // on low for 10s
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 30, 0);
    }
    timers[0] = millis();
    stage++;
  }

  if (stage == 2 && (millis() - timers[0]) > 300) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    timers[0] = millis();
    stage++;
  }

  if (stage == 3 && (millis() - timers[0]) > 700) {
    timers[0] = millis();
    stage++;
  }

  if ((stage == 4 || stage == 6) && (millis() - timers[0]) > 100) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 50, 0);
    }
    timers[0] = millis();
    stage++;
  }

  if ((stage == 5 || stage == 7) && (millis() - timers[0]) > 100) {

    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    timers[0] = millis();
    stage++;
  }

  if (stage == 8  && (millis() - timers[0]) > 500) {
    timers[0] = millis();
    stage++;
  }

  if ((stage >= 9 && stage <= (9 + 64)) && (millis() - timers[0]) > 70) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, j * 3, 0);
    }
    timers[0] = millis();
    stage++;
    j++;
  }

  if (stage > (9 + 64)) {
    stage = 0;
    j = 0;
    timers[0] = millis();
  }
}

void off_air() {
  int i = 0;
  int red = 240;  
  static int j = 0; 
  
  if (stage == 0) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 250, 0);
    }
    timers[0] = millis();
    stage++;
  }
  
  if (stage == 1 && (millis()-timers[0]) > 250) {
    // Off
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    timers[0] = millis();
    stage++;
  }

  if (stage == 2 && (millis() - timers[0]) > 75) {

    // on low for 10s
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 180, 0);
    }
    timers[0] = millis();
    stage++;
  }

  if (stage == 3 && (millis() - timers[0]) > 75) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    timers[0] = millis();
    stage++;
  }

  if (stage == 4 && (millis() - timers[0]) > 75) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 210, 0);
    }    
    timers[0] = millis();
    stage++;
  }
  
  if (stage == 5 && (millis() - timers[0]) > 350) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }    
    timers[0] = millis();
    stage++;
  }

  if (stage == 6 && (millis() - timers[0]) > 75) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 255, 0);
    }    
    timers[0] = millis();
    stage++;
  }
  
  if (stage == 7 && (millis() - timers[0]) > 75) {
    for (i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }    
    timers[0] = millis();
    stage++;
  }

  if (stage == 8 && (millis() - timers[0]) > 750) {
    stage = 0;
  }  
}

void controller_boxes() {
  
}

void loop() {
  // put your main code here, to run repeatedly:

//  on_air();
//  off_air();    
  FastLED.show();
}

