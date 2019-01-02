#include <Servo.h>
#include <math.h>

#define SET_PIN 10
#define GET_PIN A1
#define MAX_ANGLE 80
#define MIN_ANGLE 50

Servo webcam;

void setup() {
    pinMode(SET_PIN, OUTPUT);
    pinMode(GET_PIN, INPUT);
    webcam.attach(SET_PIN);
}

void loop() {
    full_loop();
}

void full_loop() {
    int knob;
    int current_angle = webcam.read();
    static int running = millis();

    if ((millis() - running) > 40) {
        knob = map(analogRead(GET_PIN), 0, 900, MIN_ANGLE, MAX_ANGLE);

        if (abs(knob - current_angle) > 3) {
            webcam.write(knob);
            running = millis();
        }
    }
}
