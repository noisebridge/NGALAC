#include <Servo.h>
#include <math.h>

#define SET_PIN 10
#define GET_PIN A1
#define MAX_ANGLE 130
#define MIN_ANGLE 30

Servo webcam;

void setup() {
    pinMode(SET_PIN, OUTPUT);
    pinMode(GET_PIN, INPUT);
    Serial.begin(9600);
    webcam.attach(SET_PIN);
}

void loop() {
    full_loop();
}

void knob_test() {

    int knob;
    knob = analogRead(GET_PIN);
    Serial.println(map(knob, 0, 900, 10, 130));

}

void servo_test() {
    static int amt = 0;
    static int incr = 1;

    // Serial.print("preread: ");Serial.println(webcam.read());
    // webcam.attach(SET_PIN);
    // delay(40);
    Serial.print("attread: ");Serial.println(webcam.read());
    webcam.write(amt);
    amt += incr;
    delay(40);
    Serial.print("pstread: ");Serial.println(webcam.read());
    // webcam.detach();
    // Serial.print("detread: ");Serial.println(webcam.read());
    // delay(40);
    if (amt >= 130) {
        incr = -1;
    }
    if (amt <= 0) {
        incr = 1;
    }
}
    
void full_loop() {
    int knob;
    int current_angle = webcam.read();
    static int running = millis();

    if ((millis() - running) > 40) {
        knob = map(analogRead(GET_PIN), 0, 900, MIN_ANGLE, MAX_ANGLE);

        if (abs(knob - current_angle) > 3) {
            Serial.print("k: ");Serial.print(knob);Serial.print("   c: ");Serial.println(current_angle);
            webcam.write(knob);
            running = millis();
        }
    }
}
