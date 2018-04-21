

#include <FastLED.h>
#define NUM_LEDS 144
#define DATA_PIN 9  // fix

/* set double speed bit in spi control register
 * w2803 - what is max datarate?
 */

HSV leds[NUM_LEDS];

void setup_leds(void) {
    FastLED.addLeds<WS2803, DATA_PIN>(leds, NUM_LEDS);
}

// Blink all

// 1- LED chase

// triangle wave of half-wave length: n leds trough-to-trough


