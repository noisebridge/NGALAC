#include <FastLED.h>
#include "led_pattern.h"

LedPattern::LedPattern(*led_array) {
    leds = &led_array;
}
    
