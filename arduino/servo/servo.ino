#include <Servo.h>

#define SET_PIN 9
#define GET_PIN 10
#define MAX_ANGLE 110
#define MIN_ANGLE 10

Servo webcam;

void setup() {
    pinMode(SET_PIN, OUTPUT);
    pinMode(GET_PIN, INPUT);
    webcam.attach(SET_PIN);
}


