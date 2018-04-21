#include <FastLED.h>

class LedPattern {

private:
    static uint8_t pattern_cycle_delay;
    static uint8_t neighbor_change_delay; 

public:
    LedPattern(HSV *led_array);
    HSV *leds;
    void tick();
    void update();
    void set_length;
    void set_
}
